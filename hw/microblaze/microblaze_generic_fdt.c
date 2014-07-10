/*
 * Model of Petalogix linux reference design for all boards
 *
 * Copyright (c) 2009 Edgar E. Iglesias.
 * Copyright (c) 2009 Michal Simek.
 * Copyright (c) 2012 Peter A.G. Crosthwaite (peter.croshtwaite@petalogix.com)
 * Copyright (c) 2012 Petalogix Pty Ltd.
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

#include "hw/sysbus.h"
#include "net/net.h"
#include "hw/block/flash.h"
#include "sysemu/sysemu.h"
#include "hw/boards.h"
#include "sysemu/device_tree.h"
#include "exec/memory.h"
#include "exec/address-spaces.h"
#include "qemu/config-file.h"

#include "hw/fdt/fdt_generic_util.h"
#include "hw/fdt/fdt_generic_devices.h"

#include "boot.h"

#define VAL(name) qemu_fdt_getprop_cell(fdt, node_path, name, 0, false, \
                                                NULL)

static void
microblaze_generic_fdt_reset(MicroBlazeCPU *cpu, void *fdt)
{
    CPUMBState *env = &cpu->env;

    char node_path[DT_PATH_LENGTH];
    qemu_fdt_get_node_by_name(fdt, node_path, "cpu@");
    int t;
    int use_exc = 0;

    env->pvr.regs[0] = 0;
    env->pvr.regs[2] = PVR2_D_OPB_MASK \
                        | PVR2_D_LMB_MASK \
                        | PVR2_I_OPB_MASK \
                        | PVR2_I_LMB_MASK \
                        | 0;

    if (VAL("xlnx,pvr")) {
        env->sregs[SR_MSR] |= MSR_PVR;
    }

    /* Even if we don't have PVR's, we fill out everything
       because QEMU will internally follow what the pvr regs
       state about the HW.  */

    if (VAL("xlnx,pvr") == 2) {
        env->pvr.regs[0] |= PVR0_PVR_FULL_MASK;
    }

    if (VAL("xlnx,endianness")) {
        env->pvr.regs[0] |= PVR0_ENDI;
    }

    if (VAL("xlnx,use-barrel")) {
        env->pvr.regs[0] |= PVR0_USE_BARREL_MASK;
        env->pvr.regs[2] |= PVR2_USE_BARREL_MASK;
    }

    if (VAL("xlnx,use-div")) {
        env->pvr.regs[0] |= PVR0_USE_DIV_MASK;
        env->pvr.regs[2] |= PVR2_USE_DIV_MASK;
    }

    t = VAL("xlnx,use-hw-mul");
    if (t) {
        env->pvr.regs[0] |= PVR0_USE_HW_MUL_MASK;
        env->pvr.regs[2] |= PVR2_USE_HW_MUL_MASK;
        if (t >= 2) {
            env->pvr.regs[2] |= PVR2_USE_MUL64_MASK;
        }
    }

    t = VAL("xlnx,use-fpu");
    if (t) {
        env->pvr.regs[0] |= PVR0_USE_FPU_MASK;
        env->pvr.regs[2] |= PVR2_USE_FPU_MASK;
        if (t > 1) {
            env->pvr.regs[2] |= PVR2_USE_FPU2_MASK;
        }
    }

    if (VAL("xlnx,use-msr-instr")) {
        env->pvr.regs[2] |= PVR2_USE_MSR_INSTR;
    }

    if (VAL("xlnx,use-pcmp-instr")) {
        env->pvr.regs[2] |= PVR2_USE_PCMP_INSTR;
    }

    if (VAL("xlnx,opcode-0x0-illegal")) {
        env->pvr.regs[2] |= PVR2_OPCODE_0x0_ILL_MASK;
    }

    if (VAL("xlnx,unaligned-exceptions")) {
        env->pvr.regs[2] |= PVR2_UNALIGNED_EXC_MASK;
        use_exc = 1;
    }

    if (VAL("xlnx,ill-opcode-exception")) {
        env->pvr.regs[2] |= PVR2_ILL_OPCODE_EXC_MASK;
        use_exc = 1;
    }

    if (VAL("xlnx,iopb-bus-exception")) {
        env->pvr.regs[2] |= PVR2_IOPB_BUS_EXC_MASK;
        use_exc = 1;
    }

    if (VAL("xlnx,dopb-bus-exception")) {
        env->pvr.regs[2] |= PVR2_DOPB_BUS_EXC_MASK;
        use_exc = 1;
    }

    if (VAL("xlnx,div-zero-exception")) {
        env->pvr.regs[2] |= PVR2_DIV_ZERO_EXC_MASK;
        use_exc = 1;
    }

    if (VAL("xlnx,fpu-exception")) {
        env->pvr.regs[2] |= PVR2_FPU_EXC_MASK;
        use_exc = 1;
    }

    env->pvr.regs[0] |= VAL("xlnx,pvr-user1") & 0xff;
    env->pvr.regs[1] = VAL("xlnx,pvr-user2");

    /* MMU regs.  */
    t = VAL("xlnx,use-mmu");
    if (use_exc || t) {
        env->pvr.regs[0] |= PVR0_USE_EXC_MASK ;
    }

    if (t) {
        env->pvr.regs[0] |= PVR0_USE_MMU;
    }
    env->pvr.regs[11] = t << 30;
    t = VAL("xlnx,mmu-zones");
    env->pvr.regs[11] |= t << 17;
    env->mmu.c_mmu_zones = t;

    t = VAL("xlnx,mmu-tlb-access");
    env->mmu.c_mmu_tlb_access = t;
    env->pvr.regs[11] |= t << 22;

    {
        char *str;
        const struct {
            const char *name;
            unsigned int arch;
        } arch_lookup[] = {
            {"virtex2", 0x4},
            {"virtex2pro", 0x5},
            {"spartan3", 0x6},
            {"virtex4", 0x7},
            {"virtex5", 0x8},
            {"spartan3e", 0x9},
            {"spartan3a", 0xa},
            {"spartan3an", 0xb},
            {"spartan3adsp", 0xc},
            {"spartan6", 0xd},
            {"virtex6", 0xe},
            {"spartan2", 0xf0},
            {NULL, 0},
        };
        unsigned int i = 0;

        str = qemu_fdt_getprop(fdt, node_path, "xlnx,family", NULL,
                                    false, NULL);
        while (arch_lookup[i].name && str) {
            if (strcmp(arch_lookup[i].name, str) == 0) {
                break;
            }
            i++;
        }
        if (!str || !arch_lookup[i].arch) {
            env->pvr.regs[10] = 0x0c000000; /* spartan 3a dsp family.  */
        } else {
            env->pvr.regs[10] = arch_lookup[i].arch << 24;
        }
        g_free(str);
    }

    {
        char *str;
        const struct {
            const char *name;
            unsigned int arch;
        } cpu_lookup[] = {
            /* These key value are as per MBV field in PVR0 */
            {"5.00.a", 0x01},
            {"5.00.b", 0x02},
            {"5.00.c", 0x03},
            {"6.00.a", 0x04},
            {"6.00.b", 0x06},
            {"7.00.a", 0x05},
            {"7.00.b", 0x07},
            {"7.10.a", 0x08},
            {"7.10.b", 0x09},
            {"7.10.c", 0x0a},
            {"7.10.d", 0x0b},
            {"7.20.a", 0x0c},
            {"7.20.b", 0x0d},
            {"7.20.c", 0x0e},
            {"7.20.d", 0x0f},
            {"7.30.a", 0x10},
            {"7.30.b", 0x11},
            {"8.00.a", 0x12},
            {"8.00.b", 0x13},
            {"8.10.a", 0x14},
            /* FIXME There is no keycode defined in MBV for these versions */
            {"2.10.a", 0x10},
            {"3.00.a", 0x20},
            {"4.00.a", 0x30},
            {"4.00.b", 0x40},
            {NULL, 0},
        };
        unsigned int i = 0;

        str = qemu_fdt_getprop(fdt, node_path, "model", NULL, false, NULL);

        while (cpu_lookup[i].name && str) {
            if (strcmp(cpu_lookup[i].name, str + strlen("microblaze,")) == 0) {
                break;
            }
            i++;
        }
        if (!str || !cpu_lookup[i].arch) {
            fprintf(stderr, "unable to find MicroBlaze model.\n");
            env->pvr.regs[0] |= 0xb << 8;
        } else {
            env->pvr.regs[0] |= cpu_lookup[i].arch << 8;
        }
        g_free(str);
    }

    {
        env->pvr.regs[4] = PVR4_USE_ICACHE_MASK
                           | (21 << 26) /* Tag size.  */
                           | (4 << 21)
                           | (11 << 16);
        env->pvr.regs[6] = VAL("d-cache-baseaddr");
        env->pvr.regs[7] = VAL("d-cache-highaddr");
        env->pvr.regs[5] = PVR5_USE_DCACHE_MASK
                           | (21 << 26) /* Tag size.  */
                           | (4 << 21)
                           | (11 << 16);

        if (VAL("xlnx,dcache-use-writeback")) {
            env->pvr.regs[5] |= PVR5_DCACHE_WRITEBACK_MASK;
        }

        env->pvr.regs[8] = VAL("i-cache-baseaddr");
        env->pvr.regs[9] = VAL("i-cache-highaddr");
    }
}

#define LMB_BRAM_SIZE  (128 * 1024)

#define MACHINE_NAME "microblaze-fdt"

#ifdef TARGET_WORDS_BIGENDIAN
int endian = 1;
#else
int endian;
#endif

static void microblaze_generic_fdt_init(MachineState *machine)
{
    MicroBlazeCPU *cpu;
    MemoryRegion *address_space_mem = get_system_memory();
    MemoryRegion *lmb_bram = g_new(MemoryRegion, 1);
    MemoryRegion *ddr_ram = g_new(MemoryRegion, 1);
    hwaddr ram_base;
    void *fdt = NULL;
    const char *dtb_arg;
    QemuOpts *machine_opts;

    /* for memory node */
    char node_path[DT_PATH_LENGTH];

    machine_opts = qemu_opts_find(qemu_find_opts("machine"), 0);
    if (!machine_opts) {
        goto no_dtb_arg;
    }
    dtb_arg = qemu_opt_get(machine_opts, "dtb");
    if (!dtb_arg) {
        goto no_dtb_arg;
    }

    fdt = load_device_tree(dtb_arg, NULL);
    if (!fdt) {
        hw_error("Error: Unable to load Device Tree %s\n", dtb_arg);
        return;
    }

    /* init CPUs */
    if (machine->cpu_model == NULL) {
        machine->cpu_model = "microblaze";
    }
    cpu = cpu_mb_init(machine->cpu_model);

    /* find memory node */
    /* FIXME it could be good to fix case when you don't find memory node */
    qemu_fdt_get_node_by_name(fdt, node_path, "memory@");
    ram_base = qemu_fdt_getprop_cell(fdt, node_path, "reg", 0,
                                            false, &error_abort);
    ram_size = qemu_fdt_getprop_cell(fdt, node_path, "reg", 1,
                                            false, &error_abort);

    /* FIXME: instantiate from FDT like evrything else */
    /* Attach emulated BRAM through the LMB.  */
    memory_region_init_ram(lmb_bram, NULL, "microblaze_fdt.lmb_bram", LMB_BRAM_SIZE);
    vmstate_register_ram_global(lmb_bram);
    memory_region_add_subregion(address_space_mem, 0, lmb_bram);

    memory_region_init_ram(ddr_ram, NULL, "microblaze_fdt.ddr_ram", ram_size);
    vmstate_register_ram_global(ddr_ram);
    memory_region_add_subregion(address_space_mem, ram_base, ddr_ram);

    /* Instantiate peripherals from the FDT.  */
    fdt_init_destroy_fdti(
        fdt_generic_create_machine(fdt,
                            qdev_get_gpio_in(DEVICE(cpu), MB_CPU_IRQ)));

    microblaze_load_kernel(cpu, ram_base, ram_size,
                           machine->initrd_filename,
                           NULL,
                           microblaze_generic_fdt_reset, fdt);
    return;
no_dtb_arg:
    hw_error("DTB must be specified for %s machine model\n", MACHINE_NAME);
    return;
}

static QEMUMachine microblaze_generic_fdt = {
    .name = MACHINE_NAME,
    .desc = "Petalogix FDT Generic, for all Microblaze MMU boards",
    .init = microblaze_generic_fdt_init,
};

static void microblaze_fdt_init(void)
{
    qemu_register_machine(&microblaze_generic_fdt);
}

machine_init(microblaze_fdt_init);

fdt_register_compatibility(simple_bus_fdt_init, "xlnx,compound");
fdt_register_compatibility_opaque(pflash_cfi01_fdt_init, "cfi-flash", 0,
    &endian);
