/*
 * Flexible model of microblaze designs that use a device-tree to determine
 * the hardware configuration.
 *
 * Copyright (c) 2010 LabX Technologies, LLC
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "sysbus.h"
#include "hw.h"
#include "net.h"
#include "flash.h"
#include "sysemu.h"
#include "devices.h"
#include "boards.h"
#include "device_tree.h"
#include "xilinx.h"
#include "labx_devices.h"
#include "loader.h"
#include "elf.h"
#include "blockdev.h"

#define LMB_BRAM_SIZE  (128 * 1024)

static struct
{
    uint32_t bootstrap_pc;
    uint32_t cmdline;
    uint32_t initrd;
    uint32_t fdt;
} boot_info;

static void main_cpu_reset(void *opaque)
{
    CPUState *env = opaque;

    cpu_reset(env);
    env->regs[5] = boot_info.cmdline;
    env->regs[6] = boot_info.initrd;
    env->regs[7] = boot_info.fdt;
    env->sregs[SR_PC] = boot_info.bootstrap_pc;
}

#ifndef CONFIG_FDT
#error "Device-tree support is required for this target to function"
#endif

#define BINARY_DEVICE_TREE_FILE "labx-microblaze.dtb"
static void* get_device_tree(int* fdt_size)
{
    char *path;
    void *fdt;

    /* Try the local "mb.dtb" override.  */
    fdt = load_device_tree("mb.dtb", fdt_size);
    if (!fdt) {
        path = qemu_find_file(QEMU_FILE_TYPE_BIOS, BINARY_DEVICE_TREE_FILE);
        if (path) {
            fdt = load_device_tree(path, fdt_size);
            qemu_free(path);
        }
    }

    return fdt;
}

static int labx_microblaze_load_device_tree(target_phys_addr_t addr,
                                      uint32_t ramsize,
                                      target_phys_addr_t initrd_base,
                                      target_phys_addr_t initrd_size,
                                      const char *kernel_cmdline)
{
    int fdt_size;
    void *fdt;
    int r;

    fdt = get_device_tree(&fdt_size);

    if (!fdt)
        return 0;

    if (kernel_cmdline && strlen(kernel_cmdline)) {
        r = qemu_devtree_setprop_string(fdt, "/chosen", "bootargs", kernel_cmdline);
        if (r < 0)
            fprintf(stderr, "couldn't set /chosen/bootargs\n");
    }
    cpu_physical_memory_write (addr, (void *)fdt, fdt_size);

    return fdt_size;
}

static uint64_t translate_kernel_address(void *opaque, uint64_t addr)
{
    return addr - 0x30000000LL;
}

static ram_addr_t get_dram_base(void* fdt)
{
    int root = qemu_devtree_node_offset(fdt, "/");
    int memory = qemu_devtree_subnode_offset_namelen(fdt, root, "memory", 6);
    if (memory > 0)
    {
        int reglen;
        const void* reg = qemu_devtree_getprop(fdt, memory, "reg", &reglen);

        if (reglen >= 4)
        {
            printf("DRAM base %08X, size %08X\n", qemu_devtree_int_array_index(reg, 0),
                qemu_devtree_int_array_index(reg, 1));
            return qemu_devtree_int_array_index(reg, 0);
        }
    }

    printf("DRAM base not found. Defaulting to 0x88000000\n");

    return 0x88000000; // Default to something reasonable
}

typedef void (*device_init_func_t)(void* fdt, int node);

typedef struct 
{
    device_init_func_t probe;
    int pass;
    const char** compat;

} devinfo_t;

/*
 * Xilinx interrupt controller device
 */
CPUState *env;
static qemu_irq irq[32] = {};
static qemu_irq *cpu_irq = NULL;

static void xilinx_interrupt_controller_probe(void* fdt, int node)
{
    int i;
    DeviceState *dev;
    int reglen;
    const void* reg = qemu_devtree_getprop(fdt, node, "reg", &reglen);
    uint32_t irq_addr = qemu_devtree_int_array_index(reg, 0);
    int nrIrqLen;
    const void* nrIrq = qemu_devtree_getprop(fdt, node, "xlnx,num-intr-inputs", &nrIrqLen);
    uint32_t nrIrqs = qemu_devtree_int_array_index(nrIrq, 0);

    printf("  IRQ BASE %08X NIRQS %d\n", irq_addr, nrIrqs);

    cpu_irq = microblaze_pic_init_cpu(env);
    dev = xilinx_intc_create(irq_addr, cpu_irq[0], 2);
    for (i = 0; i < nrIrqs; i++) {
        irq[i] = qdev_get_gpio_in(dev, i);
    }
}

devinfo_t xilinx_interrupt_controller_device = {
    .probe = &xilinx_interrupt_controller_probe,
    .pass = 0,
    .compat = (const char*[]){ "xlnx,xps-intc-1.00.a", NULL }
};

/*
 * Flash device
 */
static void flash_probe(void* fdt, int node)
{
    int reglen;
    const void* reg = qemu_devtree_getprop(fdt, node, "reg", &reglen);
    uint32_t flash_addr = qemu_devtree_int_array_index(reg, 0);
    uint32_t flash_size = qemu_devtree_int_array_index(reg, 1);

    ram_addr_t phys_flash = qemu_ram_alloc(NULL, qemu_devtree_get_name(fdt, node, NULL), flash_size);
    DriveInfo *dinfo = drive_get(IF_PFLASH, 0, 0);
    pflash_cfi01_register(qemu_devtree_int_array_index(reg, 0), phys_flash,
                          dinfo ? dinfo->bdrv : NULL, (64 * 1024),
                          flash_size >> 16,
                          1, 0x89, 0x18, 0x0000, 0x0, 1);
    printf("-- loaded %d bytes to %08X\n", load_image_targphys(qemu_devtree_get_name(fdt, node, NULL), flash_addr, flash_size), flash_addr);
}

devinfo_t flash_device = {
    .probe = &flash_probe,
    .pass = 1,
    .compat = (const char*[]){ "cfi-flash", NULL }
};

/*
 * Xilinx timer device
 */
static void xilinx_timer_probe(void* fdt, int node)
{
    int reglen;
    const void* reg = qemu_devtree_getprop(fdt, node, "reg", &reglen);
    uint32_t timer_addr = qemu_devtree_int_array_index(reg, 0);
    int irqLen;
    const void* irqs = qemu_devtree_getprop(fdt, node, "interrupts", &irqLen);
    uint32_t timer_irq = qemu_devtree_int_array_index(irqs, 0);

    printf("  TIMER BASE %08X IRQ %d\n", timer_addr, timer_irq);

    /* 2 timers at irq 2 @ 62 Mhz.  */
    xilinx_timer_create(timer_addr, irq[timer_irq], 2, 62 * 1000000);
}

devinfo_t xilinx_timer_device = {
    .probe = &xilinx_timer_probe,
    .pass = 1,
    .compat = (const char*[]){ "xlnx,xps-timer-1.00.a", NULL }
};

/*
 * Xilinx uartlite device
 */
static void xilinx_uartlite_probe(void* fdt, int node)
{
    int reglen;
    const void* reg = qemu_devtree_getprop(fdt, node, "reg", &reglen);
    uint32_t uart_addr = qemu_devtree_int_array_index(reg, 0);
    int irqLen;
    const void* irqs = qemu_devtree_getprop(fdt, node, "interrupts", &irqLen);
    uint32_t uart_irq = qemu_devtree_int_array_index(irqs, 0);

    printf("  UART BASE %08X IRQ %d\n", uart_addr, uart_irq);

    sysbus_create_simple("xilinx,uartlite", uart_addr, irq[uart_irq]);
}

devinfo_t xilinx_uartlite_device = {
    .probe = &xilinx_uartlite_probe,
    .pass = 1,
    .compat = (const char*[]){ "xlnx,xps-uartlite-1.00.a", NULL }
};

/*
 * Xilinx ethernetlite device
 */
static void xilinx_ethlite_probe(void* fdt, int node)
{
    int reglen;
    const void* reg = qemu_devtree_getprop(fdt, node, "reg", &reglen);
    uint32_t eth_addr = qemu_devtree_int_array_index(reg, 0);
    int irqLen;
    const void* irqs = qemu_devtree_getprop(fdt, node, "interrupts", &irqLen);
    uint32_t eth_irq = qemu_devtree_int_array_index(irqs, 0);

    xilinx_ethlite_create(&nd_table[0], eth_addr, irq[eth_irq], 0, 0);
}

devinfo_t xilinx_ethlite_device = {
    .probe = &xilinx_ethlite_probe,
    .pass = 1,
    .compat = (const char*[]){ "xlnx,xps-ethernetlite-2.00.b", NULL }
};

/*
 * LabX audio packetizer device
 */
static void labx_audio_packetizer_probe(void* fdt, int node)
{
    int reglen;
    const void* reg = qemu_devtree_getprop(fdt, node, "reg", &reglen);
    uint32_t packetizer_addr = qemu_devtree_int_array_index(reg, 0);
    int irqLen;
    const void* irqs = qemu_devtree_getprop(fdt, node, "interrupts", &irqLen);
    uint32_t packetizer_irq = qemu_devtree_int_array_index(irqs, 0);
    int clockLen;
    const void* clocks = qemu_devtree_getprop(fdt, node, "xlnx,num-clock-domains", &clockLen);
    uint32_t clockDomains = qemu_devtree_int_array_index(clocks, 0);
    int cacheLen;
    const void* caches = qemu_devtree_getprop(fdt, node, "xlnx,cache-data-words", &cacheLen);
    uint32_t cacheWords = qemu_devtree_int_array_index(caches, 0);

    labx_audio_packetizer_create(packetizer_addr, irq[packetizer_irq], clockDomains, cacheWords);
}

devinfo_t labx_audio_packetizer_device = {
    .probe = &labx_audio_packetizer_probe,
    .pass = 1,
    .compat = (const char*[]){ "xlnx,labx-audio-packetizer-1.00.a", NULL }
};

/*
 * LabX audio depacketizer device
 */
static void labx_audio_depacketizer_probe(void* fdt, int node)
{
    int reglen;
    const void* reg = qemu_devtree_getprop(fdt, node, "reg", &reglen);
    uint32_t depacketizer_addr = qemu_devtree_int_array_index(reg, 0);
    int irqLen;
    const void* irqs = qemu_devtree_getprop(fdt, node, "interrupts", &irqLen);
    uint32_t depacketizer_irq = qemu_devtree_int_array_index(irqs, 0);
    int clockLen;
    const void* clocks = qemu_devtree_getprop(fdt, node, "xlnx,num-clock-domains", &clockLen);
    uint32_t clockDomains = qemu_devtree_int_array_index(clocks, 0);
    int cacheLen;
    const void* caches = qemu_devtree_getprop(fdt, node, "xlnx,cache-data-words", &cacheLen);
    uint32_t cacheWords = qemu_devtree_int_array_index(caches, 0);
    int ifLen;
    const void* ifType = qemu_devtree_getprop(fdt, node, "xlnx,interface-type", &ifLen);
    int hasDMA = (0 != strncmp("CACHE_RAM", (const char*)ifType, ifLen));

    labx_audio_depacketizer_create(depacketizer_addr, irq[depacketizer_irq], clockDomains, cacheWords, hasDMA);
}

devinfo_t labx_audio_depacketizer_device = {
    .probe = &labx_audio_depacketizer_probe,
    .pass = 1,
    .compat = (const char*[]){ "xlnx,labx-audio-depacketizer-1.00.a", NULL }
};

/*
 * LabX dma device
 */
static void labx_dma_probe(void* fdt, int node)
{
    int reglen;
    const void* reg = qemu_devtree_getprop(fdt, node, "reg", &reglen);
    uint32_t dma_addr = qemu_devtree_int_array_index(reg, 0);

    labx_dma_create(dma_addr, 1024);
}

devinfo_t labx_dma_device = {
    .probe = &labx_dma_probe,
    .pass = 1,
    .compat = (const char*[]){ "xlnx,labx-dma-1.00.a", "xlnx,labx-dma-1.01.a", NULL }
};

/*
 * Table of available devices
 */
devinfo_t* devices[] = {
    &xilinx_interrupt_controller_device,
    &flash_device,
    &xilinx_timer_device,
    &xilinx_uartlite_device,
    &xilinx_ethlite_device,
    &labx_audio_packetizer_device,
    &labx_audio_depacketizer_device,
    &labx_dma_device,
    NULL
};

static int plb_device_probe(void* fdt, int node, int pass)
{
    devinfo_t **dev = &(devices[0]);

    while (*dev)
    {
        const char** compat = &((*dev)->compat[0]);
        while (*compat)
        {
            if (0 == qemu_devtree_node_check_compatible(fdt, node, *compat))
            {
                if (pass == (*dev)->pass)
                {
                    printf("Adding a device for node %s\n", qemu_devtree_get_name(fdt, node, NULL));

                    (*dev)->probe(fdt, node);
                    return 0;
                }

                if (pass < (*dev)->pass)
                {
                    /* Probe again on the next pass */
                    return 1;
                }
            }

            compat++;
        }

        dev++;
    }

    return 0;
}

static void plb_bus_probe(void* fdt)
{
    int root = qemu_devtree_node_offset(fdt, "/");
    int plb = qemu_devtree_subnode_offset_namelen(fdt, root, "plb", 3);
    if (plb > 0)
    {
        /* Do multiple passes through the devices. Some have dependencies on others being first */
        int pass = 0;
        int again = 0;
        do
        {
            int child = plb;
            again = 0;
            do
            {
                child = qemu_devtree_next_child_offset(fdt, plb, child);
                if (child < 0)
                    break;

                again |= plb_device_probe(fdt, child, pass);

            } while (1);

            pass++;

        } while (again);
    }
}

static void
labx_microblaze_init(ram_addr_t ram_size,
                          const char *boot_device,
                          const char *kernel_filename,
                          const char *kernel_cmdline,
                          const char *initrd_filename, const char *cpu_model)
{
    int kernel_size;
    int fdt_size;
    void* fdt = get_device_tree(&fdt_size);
    target_phys_addr_t ddr_base = get_dram_base(fdt);
    ram_addr_t phys_lmb_bram;
    ram_addr_t phys_ram;

    /* init CPUs */
    if (cpu_model == NULL) {
        cpu_model = "microblaze";
    }
    env = cpu_init(cpu_model);

    env->pvr.regs[10] = 0x0c000000; /* spartan 3a dsp family.  */
    qemu_register_reset(main_cpu_reset, env);

    /* Attach emulated BRAM through the LMB. LMB size is not specified in the device-tree
       but there must be one to hold the vector table. */
    phys_lmb_bram = qemu_ram_alloc(NULL, "labx_microblaze.lmb_bram",
                                   LMB_BRAM_SIZE);
    cpu_register_physical_memory(0x00000000, LMB_BRAM_SIZE,
                                 phys_lmb_bram | IO_MEM_RAM);

    phys_ram = qemu_ram_alloc(NULL, "labx_microblaze.ram", ram_size);
    cpu_register_physical_memory(ddr_base, ram_size, phys_ram | IO_MEM_RAM);

    /* Create other devices listed in the device-tree */
    plb_bus_probe(fdt);

    if (kernel_filename) {
        uint64_t entry, low, high;
        uint32_t base32;

        /* Boots a kernel elf binary.  */
        kernel_size = load_elf(kernel_filename, NULL, NULL,
                               &entry, &low, &high,
                               1, ELF_MACHINE, 0);
        base32 = entry;
        if (base32 == 0xc0000000) {
            kernel_size = load_elf(kernel_filename, translate_kernel_address,
                                   NULL, &entry, NULL, NULL,
                                   1, ELF_MACHINE, 0);
        }
        /* Always boot into physical ram.  */
        boot_info.bootstrap_pc = ddr_base + (entry & 0x07ffffff);

        /* If it wasn't an ELF image, try an u-boot image.  */
        if (kernel_size < 0) {
            target_phys_addr_t uentry, loadaddr;

            kernel_size = load_uimage(kernel_filename, &uentry, &loadaddr, 0);
            boot_info.bootstrap_pc = uentry;
            high = (loadaddr + kernel_size + 3) & ~3;
        }

        /* Not an ELF image nor an u-boot image, try a RAW image.  */
        if (kernel_size < 0) {
            kernel_size = load_image_targphys(kernel_filename, ddr_base,
                                              ram_size);
            boot_info.bootstrap_pc = ddr_base;
            high = (ddr_base + kernel_size + 3) & ~3;
        }

	if (initrd_filename)
	{
		uint32_t initrd_base = 0x88c00000;
		uint32_t initrd_size = load_image_targphys(initrd_filename, initrd_base,
			ram_size - initrd_base);
		if (initrd_size <= 0) {
			fprintf(stderr, "qemu: could not load initial ram disk '%s'\n",
			initrd_filename);
			exit(1);
		}

		boot_info.initrd = initrd_base;
	}
	else
	{
		boot_info.initrd = 0x00000000;
	}

        boot_info.cmdline = high + 4096;
        if (kernel_cmdline && strlen(kernel_cmdline)) {
            pstrcpy_targphys("cmdline", boot_info.cmdline, 256, kernel_cmdline);
        }
        /* Provide a device-tree.  */
        boot_info.fdt = boot_info.cmdline + 4096; 
        labx_microblaze_load_device_tree(boot_info.fdt, ram_size,
                                   0, 0,
                                   kernel_cmdline);
    }
}

static QEMUMachine labx_microblaze_machine = {
    .name = "labx-microblaze-devicetree",
    .desc = "Microblaze design based on the peripherals specified in the device-tree.",
    .init = labx_microblaze_init,
    .is_default = 1
};

static void labx_microblaze_machine_init(void)
{
    qemu_register_machine(&labx_microblaze_machine);
}

machine_init(labx_microblaze_machine_init);

