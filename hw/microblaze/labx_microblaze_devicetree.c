/*
 * Flexible model of microblaze designs that use a device-tree to determine
 * the hardware configuration.
 *
 * Copyright (c) 2010 LabX Technologies, LLC
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
#include "target/microblaze/cpu.h"
#include "hw/loader.h"
#include "elf.h"
#include "sysemu/blockdev.h"
#include "exec/memory.h"
#include "exec/address-spaces.h"
#include "qemu/config-file.h"
#include "qapi/error.h"

#include "hw/fdt_generic.h"
#include "hw/fdt_generic_devices.h"
#include "hw/fdt_generic_util.h"

#include <libfdt.h>

#define LMB_BRAM_SIZE  (128 * 1024)

static int endian;

static struct
{
    uint32_t bootstrap_pc;
    uint32_t cmdline;
    uint32_t initrd;
    uint32_t fdt;
} boot_info;

static void main_cpu_reset(void *opaque)
{
    MicroBlazeCPU *cpu = opaque;
    CPUMBState *env = &cpu->env;

    cpu_reset(CPU(cpu));

    env->regs[5] = boot_info.cmdline;
    env->regs[6] = boot_info.initrd;
    env->regs[7] = boot_info.fdt;
    env->sregs[SR_PC] = boot_info.bootstrap_pc;
}

#ifndef CONFIG_FDT
#error "Device-tree support is required for this target to function"
#endif

#define BINARY_DEVICE_TREE_FILE "labx-microblaze.dtb"
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

static int labx_microblaze_load_device_tree(hwaddr addr,
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
    return addr - 0x30000000LL;
}

static ram_addr_t get_dram_base(void *fdt)
{
    Error *errp = NULL;
     
    printf("DRAM base %08X, size %08X\n",
        qemu_fdt_getprop_cell(fdt, "/memory", "reg", NULL, 0, 0, &errp),
        qemu_fdt_getprop_cell(fdt, "/memory", "reg", NULL, 1, 0, &errp));
     
    return qemu_fdt_getprop_cell(fdt, "/memory", "reg", NULL, 0, 0, &errp);
}

/*
 * Xilinx interrupt controller device
 */
static void labx_microblaze_init(MachineState *machine)
{
    MemoryRegion *address_space_mem = get_system_memory();

    int kernel_size;
    int fdt_size;
    void *fdt = get_device_tree(&fdt_size);
    hwaddr ddr_base = get_dram_base(fdt);
    MemoryRegion *phys_lmb_bram = g_new(MemoryRegion, 1);
    MemoryRegion *phys_ram = g_new(MemoryRegion, 1);

    /* init CPUs */
    if (machine->cpu_model == NULL) {
        machine->cpu_model = "microblaze";
    }

    /* Attach emulated BRAM through the LMB. LMB size is not specified in the
       device-tree but there must be one to hold the vector table. */
    memory_region_init_ram(phys_lmb_bram, NULL, "labx_microblaze.lmb_bram",
                           LMB_BRAM_SIZE, &error_fatal);
    memory_region_add_subregion(address_space_mem, 0x00000000, phys_lmb_bram);

    memory_region_init_ram(phys_ram, NULL, "labx_microblaze.ram", ram_size, &error_fatal);
    memory_region_add_subregion(address_space_mem, ddr_base, phys_ram);

    /* Create other devices listed in the device-tree */
    fdt_init_destroy_fdti(fdt_generic_create_machine(fdt, NULL));

    /* Register reset now that the CPU has been created */
    qemu_register_reset(main_cpu_reset, first_cpu);

    if (machine->kernel_filename) {
        uint64_t entry, low, high;
        uint32_t base32;

        /* Boots a kernel elf binary.  */
        kernel_size = load_elf(machine->kernel_filename, NULL, NULL,
                               &entry, &low, &high,
                               1, EM_MICROBLAZE, 0, 0);
        base32 = entry;
        if (base32 == 0xc0000000) {
            kernel_size = load_elf(machine->kernel_filename, translate_kernel_address,
                                   NULL, &entry, NULL, NULL,
                                   1, EM_MICROBLAZE, 0, 0);
        }
        /* Always boot into physical ram.  */
        boot_info.bootstrap_pc = ddr_base + (entry & 0x07ffffff);

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
        labx_microblaze_load_device_tree(boot_info.fdt, ram_size,
                                         0, 0,
                                         machine->kernel_cmdline);
    }
}

static void labx_microblaze_devicetree_machine_init(MachineClass *mc)
{
    mc->desc = "MicroBlaze design based on the peripherals specified in the device-tree.";
    mc->init = labx_microblaze_init;
    mc->is_default = 1;
}

DEFINE_MACHINE("labx-microblaze-devicetree", labx_microblaze_devicetree_machine_init)

fdt_register_compatibility_opaque(pflash_cfi01_fdt_init, "compatible:cfi-flash", 0, &endian);
