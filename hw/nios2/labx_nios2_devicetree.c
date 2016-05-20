/*
 * Flexible model of nios2 designs that use a device-tree to determine
 * the hardware configuration.
 *
 * Copyright (c) 2012 Chris Wulff <crwulff@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see
 * <http://www.gnu.org/licenses/lgpl-2.1.html>
 */

#include "qemu/osdep.h"
#include "hw/sysbus.h"
#include "hw/hw.h"
#include "net/net.h"
#include "hw/block/flash.h"
#include "sysemu/sysemu.h"
#include "hw/devices.h"
#include "hw/boards.h"
#include "sysemu/device_tree.h"
#include "target-nios2/cpu.h"
#include "nios2.h"
#include "hw/loader.h"
#include "elf.h"
#include "sysemu/blockdev.h"
#include "exec/memory.h"
#include "exec/address-spaces.h"
#include "qemu/config-file.h"
#include "qapi/error.h"

#include "hw/fdt/fdt_generic.h"
#include "hw/fdt/fdt_generic_devices.h"
#include "hw/fdt/fdt_generic_util.h"

#include <libfdt.h>

#define LMB_BRAM_SIZE  (128 * 1024)

static int endian; /* Always little endian */

static int sopc_device_probe(FDTMachineInfo *fdti, const char *node_path, int pass, uint32_t offset);

static struct {
    uint32_t bootstrap_pc;
    uint32_t cmdline;
    uint32_t initrd;
    uint32_t fdt;
} boot_info;

static void main_cpu_reset(void *opaque)
{
    Nios2CPU *cpu = opaque;
    CPUNios2State *env = &cpu->env;

    cpu_reset(CPU(cpu));

    env->regs[R_ARG0] = boot_info.cmdline;
    env->regs[R_ARG1] = boot_info.initrd;
    env->regs[R_ARG2] = boot_info.fdt;
    env->regs[R_PC]   = boot_info.bootstrap_pc;
}

#ifndef CONFIG_FDT
#error "Device-tree support is required for this target to function"
#endif

#define BINARY_DEVICE_TREE_FILE "labx-nios2.dtb"
static void *get_device_tree(int *fdt_size)
{
    char *path;
    void *fdt;
    const char *dtb_arg;
    QemuOpts *machine_opts;

    machine_opts = qemu_opts_find(qemu_find_opts("machine"), 0);
    if (!machine_opts) {
        dtb_arg = BINARY_DEVICE_TREE_FILE;
    } else {
      dtb_arg = qemu_opt_get(machine_opts, "dtb");
      if (!dtb_arg) {
          dtb_arg = BINARY_DEVICE_TREE_FILE;
      }
    }

    fdt = load_device_tree(dtb_arg, fdt_size);
    if (!fdt) {
        path = qemu_find_file(QEMU_FILE_TYPE_BIOS, BINARY_DEVICE_TREE_FILE);
        if (path) {
            fdt = load_device_tree(path, fdt_size);
            g_free(path);
        }
    }

    return fdt;
}

static int labx_load_device_tree(hwaddr addr,
                                 uint32_t ramsize,
                                 hwaddr initrd_base,
                                 hwaddr initrd_size,
                                 const char *kernel_cmdline)
{
    int fdt_size;
    void *fdt;
    int r;

    fdt = get_device_tree(&fdt_size);

    if (!fdt) {
        return 0;
    }

    if (kernel_cmdline && strlen(kernel_cmdline)) {
        r = qemu_fdt_setprop_string(fdt, "/chosen", "bootargs",
                                        kernel_cmdline);
        if (r < 0) {
            fprintf(stderr, "couldn't set /chosen/bootargs\n");
        }
    }
    cpu_physical_memory_write(addr, (void *)fdt, fdt_size);

    return fdt_size;
}

static uint64_t translate_kernel_address(void *opaque, uint64_t addr)
{
    return addr - 0xC0000000LL;
}

static ram_addr_t get_dram_base(void *fdt)
{
    Error *errp = NULL;

    printf("DRAM base %08X, size %08X\n",
        qemu_fdt_getprop_cell(fdt, "/memory", "reg", NULL, 0, 0, &errp),
        qemu_fdt_getprop_cell(fdt, "/memory", "reg", NULL, 1, 0, &errp));

    return qemu_fdt_getprop_cell(fdt, "/memory", "reg", NULL, 0, 0, &errp);
}

typedef void (*device_init_func_t)(FDTMachineInfo *fdti, const char *node_path, uint32_t offset);

typedef struct DevInfo {
    device_init_func_t probe;
    int pass;
    const char **compat;

} DevInfo;

/*
 * Interrupt controller device
 */

static void cpu_probe(FDTMachineInfo *fdti, const char *node_path, uint32_t offset)
{
    //int i;
    DeviceState *dev;
    Error *errp = NULL;

    Nios2CPU *cpu = cpu_nios2_init("nios2");

    qemu_register_reset(main_cpu_reset, cpu);

#if 0 /* TODO: Finish off the vectored-interrupt-controller */
    int reglen;
    const void *reg = qemu_fdt_getprop_offset(fdt, node, "reg", &reglen);
    uint32_t irq_addr = qemu_fdt_int_array_index(reg, 0) + offset;
    int nrIrqLen;
    const void *nrIrq =
        qemu_fdt_getprop_offset(fdt, node, "ALTR,num-intr-inputs",
                                    &nrIrqLen);
    uint32_t nrIrqs = qemu_fdt_int_array_index(nrIrq, 0);

    printf("  IRQ BASE %08X NIRQS %d\n", irq_addr, nrIrqs);

    fdti->irq_base = qdev_get_gpio_in(DEVICE(cpu), NIOS2_CPU_IRQ);
    dev = altera_vic_create(irq_addr, fdti->irq_base, 2);
#else
    /* Internal interrupt controller (IIC) */
    fdti->irq_base = qdev_get_gpio_in(DEVICE(cpu), NIOS2_CPU_IRQ);
    dev = altera_iic_create(cpu, fdti->irq_base, 2);
#endif

    /* TODO: use the entrypoint of the passed in elf file or
       the device-tree one */
#if 0
    cpu->env.reset_addr =
        qemu_fdt_getprop_cell(fdt, node_path, "ALTR,reset-addr", NULL, 0, 0, &errp);
#else
    cpu->env.reset_addr = 0xc0000000;
#endif

    cpu->env.exception_addr =
        qemu_fdt_getprop_cell(fdti->fdt, node_path, "ALTR,exception-addr", NULL, 0, 0, &errp);
    cpu->env.fast_tlb_miss_addr =
        qemu_fdt_getprop_cell(fdti->fdt, node_path, "ALTR,fast-tlb-miss-addr", NULL, 0, 0, &errp);

    /* reset again to use the new reset vector */
    cpu_reset(CPU(cpu));

    fdt_init_set_opaque(fdti, node_path, dev);
}

DevInfo cpu_device = {
    .probe = &cpu_probe,
    .pass = 0,
    .compat = (const char * []) {
        "ALTR,nios2-11.0",
        "ALTR,nios2-11.1",
        "ALTR,nios2-12.0",
        NULL
    }
};

/*
 * Table of available devices
 */
DevInfo *devices[] = {
    &cpu_device,
    NULL
};

static int sopc_device_probe(FDTMachineInfo *fdti, const char *node_path, int pass, uint32_t offset)
{
    DevInfo **dev = &(devices[0]);

    while (*dev) {
        const char **compat = &((*dev)->compat[0]);
        while (*compat) {
            if (0 == fdt_node_check_compatible(fdti->fdt, fdt_path_offset(fdti->fdt, node_path), *compat)) {
                if (pass == (*dev)->pass) {
                    printf("Adding a device for node %s\n",
                           fdt_get_name(fdti->fdt, fdt_path_offset(fdti->fdt, node_path), NULL));

                    (*dev)->probe(fdti, node_path, offset);
                    return 0;
                }

                if (pass < (*dev)->pass) {
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

static int cpus_probe(char *node_path, FDTMachineInfo *fdti, void *opaque)
{
    int num_children = qemu_fdt_get_num_children(fdti->fdt, "/cpus", 1);
    char **children = qemu_fdt_get_children(fdti->fdt, "/cpus", 1);
    int i;

    for (i = 0; i < num_children; i++) {
        sopc_device_probe(fdti, children[i], 0, 0xE0000000);
    }

    fdti->sysbus_base = 0xE0000000;

    return 0;
}

static void labx_nios2_init(MachineState *machine)
{
    MemoryRegion *address_space_mem = get_system_memory();

    int kernel_size;
    int fdt_size;
    void *fdt = get_device_tree(&fdt_size);
    hwaddr ddr_base = get_dram_base(fdt);
    MemoryRegion *phys_lmb_bram = g_new(MemoryRegion, 1);
    MemoryRegion *phys_ram = g_new(MemoryRegion, 1);
    MemoryRegion *phys_ram_alias = g_new(MemoryRegion, 1);

    /* Attach emulated BRAM through the LMB. LMB size is not specified
       in the device-tree but there must be one to hold the vector table. */
    memory_region_init_ram(phys_lmb_bram, NULL, "nios2.lmb_bram", LMB_BRAM_SIZE, &error_fatal);
    vmstate_register_ram_global(phys_lmb_bram);
    memory_region_add_subregion(address_space_mem, 0x00000000, phys_lmb_bram);

    memory_region_init_ram(phys_ram, NULL, "nios2.ram", ram_size, &error_fatal);
    vmstate_register_ram_global(phys_ram);
    memory_region_add_subregion(address_space_mem, ddr_base, phys_ram);
    memory_region_init_alias(phys_ram_alias, NULL, "nios2.ram.mirror",
                             phys_ram, 0, ram_size);
    memory_region_add_subregion(address_space_mem, ddr_base + 0xc0000000,
                                phys_ram_alias);

    /* Create cpus listed in the device-tree */
    add_to_force_table(cpus_probe, "cpu-probe", NULL);

    /* Create other devices listed in the device-tree */
    fdt_init_destroy_fdti(fdt_generic_create_machine(fdt, NULL));

    if (machine->kernel_filename) {
        uint64_t entry = 0, low = 0, high = 0;
        uint32_t base32 = 0;

        /* Boots a kernel elf binary.  */
        kernel_size = load_elf(machine->kernel_filename, NULL, NULL,
                               &entry, &low, &high,
                               0, EM_ALTERA_NIOS2, 0, 0);
        base32 = entry;
        if (base32 == 0xc0000000) {
            kernel_size = load_elf(machine->kernel_filename, translate_kernel_address,
                                   NULL, &entry, NULL, NULL,
                                   0, EM_ALTERA_NIOS2, 0, 0);
        }
        /* Always boot into physical ram.  */
        boot_info.bootstrap_pc = ddr_base + 0xc0000000 + (entry & 0x07ffffff);

        /* If it wasn't an ELF image, try an u-boot image.  */
        if (kernel_size < 0) {
            hwaddr uentry, loadaddr;

            kernel_size = load_uimage(machine->kernel_filename, &uentry, &loadaddr, 0,
                                      NULL, NULL);
            boot_info.bootstrap_pc = uentry;
            high = (loadaddr + kernel_size + 3) & ~3;
        }

        /* Not an ELF image nor an u-boot image, try a RAW image.  */
        if (kernel_size < 0) {
            kernel_size = load_image_targphys(machine->kernel_filename, ddr_base,
                                              ram_size);
            boot_info.bootstrap_pc = ddr_base;
            high = (ddr_base + kernel_size + 3) & ~3;
        }

        if (machine->initrd_filename) {
            uint32_t initrd_base = 0x88c00000;
            uint32_t initrd_size =
                load_image_targphys(machine->initrd_filename, initrd_base,
                                    ram_size - initrd_base);
            if (initrd_size <= 0) {
                fprintf(stderr, "qemu: could not load initial ram disk '%s'\n",
                        machine->initrd_filename);
                exit(1);
            }

            boot_info.initrd = initrd_base;
        } else {
            boot_info.initrd = 0x00000000;
        }

        boot_info.cmdline = high + 4096;
        if (machine->kernel_cmdline && strlen(machine->kernel_cmdline)) {
            pstrcpy_targphys("cmdline", boot_info.cmdline, 256, machine->kernel_cmdline);
        }
        /* Provide a device-tree.  */
        boot_info.fdt = boot_info.cmdline + 4096;
        labx_load_device_tree(boot_info.fdt, ram_size,
                              0, 0,
                              machine->kernel_cmdline);
    }
}

static void labx_nios2_devicetree_machine_init(MachineClass *mc)
{
    mc->desc = "Nios II design based on the peripherals specified in the device-tree.";
    mc->init = labx_nios2_init;
    mc->is_default = 1;
}

DEFINE_MACHINE("labx-nios2-devicetree", labx_nios2_devicetree_machine_init)

fdt_register_compatibility_opaque(pflash_cfi01_fdt_init, "cfi-flash", 0, &endian);
