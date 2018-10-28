/*
 * Copyright (c) 2016 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "qemu/osdep.h"
#include "qapi/error.h"
#include "qemu/error-report.h"
#include "qemu/option.h"
#include "cpu.h"
#include "hw/sysbus.h"
#include "hw/hw.h"
#include "hw/block/flash.h"
#include "sysemu/sysemu.h"
#include "hw/boards.h"
#include "hw/loader.h"
#include "elf.h"
#include "exec/address-spaces.h"
#include "hw/nios2/altera_juart.h"
#include "hw/nios2/altera_timer.h"
#include "hw/nios2/nios2_iic.h"

#define DEBUG

#ifdef DEBUG
# define DPRINTF(format, ...)     printf(format, ## __VA_ARGS__)
#else
# define DPRINTF(format, ...)     do { } while (0)
#endif

static struct {
    uint32_t bootstrap_pc;
} boot_info_zephyr;

static void main_cpu_reset(void *opaque)
{
    Nios2CPU *cpu = opaque;
    CPUNios2State *env = &cpu->env;

    cpu_reset(CPU(cpu));
    env->regs[R_PC] = boot_info_zephyr.bootstrap_pc;
}

#define ROM_BASE 0x00000000
#define ROM_SIZE 32

#define RAM_BASE 0x400000
#define RAM_SIZE 180224

#define TIMER_0_BASE 0x440200
#define TIMER_0_FREQ 50000000
#define TIMER_0_IRQ 2

#define JTAG_UART_0_BASE 0x201000
#define JTAG_UART_0_IRQ  0

#define ALT_CPU_EXCEPTION_ADDR  0x00400020
#define ALT_CPU_RESET_ADDR      0x00000000

static void altera_10m50_zephyr_init(MachineState *machine)
{
    const char *kernel_filename;
    MemoryRegion *sysmem = get_system_memory();
    Nios2CPU *cpu;
    int i;
    QemuOpts *machine_opts;
    int kernel_size;
    qemu_irq irq[32];

    MemoryRegion *rom = g_new(MemoryRegion, 1);
    MemoryRegion *ram = g_new(MemoryRegion, 1);

    cpu = NIOS2_CPU(object_new(TYPE_NIOS2_CPU));
    object_property_set_bool(OBJECT(cpu), false, "mmu_present",
                             &error_abort);

    object_property_set_bool(OBJECT(cpu), true, "realized", &error_abort);
    machine_opts = qemu_get_machine_opts();
    kernel_filename = qemu_opt_get(machine_opts, "kernel");

    memory_region_init_ram(rom, NULL, "nios2.rom", ROM_SIZE, &error_fatal);
    vmstate_register_ram_global(rom);
    memory_region_set_readonly(rom, true);
    memory_region_add_subregion(sysmem, ROM_BASE, rom);

    memory_region_init_ram(ram, NULL, "nios2.ram", RAM_SIZE, &error_fatal);
    vmstate_register_ram_global(ram);
    memory_region_add_subregion(sysmem, RAM_BASE, ram);

    cpu->reset_addr = ALT_CPU_RESET_ADDR;
    cpu->exception_addr = ALT_CPU_EXCEPTION_ADDR;
    cpu->fast_tlb_miss_addr  = 0;

    DPRINTF("\tcpu->reset_addr: \t\t%0x\n", cpu->reset_addr);
    DPRINTF("\tcpu->exception_addr: \t%0x\n", cpu->exception_addr);

    nios2_iic_create(cpu);

    /* Nios2 IIC has 32 interrupt-request inputs*/
    for (i = 0; i < 32; i++) {
        irq[i] = qdev_get_gpio_in(cpu->env.pic_state, i);
    }

    altera_juart_create(0, JTAG_UART_0_BASE, irq[JTAG_UART_0_IRQ]);
    altera_timer_create(TIMER_0_BASE, irq[TIMER_0_IRQ], TIMER_0_FREQ);

    cpu->reset_addr = ALT_CPU_RESET_ADDR;
    cpu->exception_addr = ALT_CPU_EXCEPTION_ADDR;
    cpu->fast_tlb_miss_addr = ALT_CPU_RESET_ADDR;

    if (kernel_filename) {
        uint64_t entry;

        /* Boots a kernel elf binary.  */
        kernel_size = load_elf(kernel_filename, NULL, NULL,
                               &entry, NULL, NULL,
                               0, EM_ALTERA_NIOS2, 0, 0);

        boot_info_zephyr.bootstrap_pc = entry;

        /* Not an ELF image, try a RAW image.  */
        if (kernel_size < 0) {
            hwaddr uentry, loadaddr;

            kernel_size = load_uimage(kernel_filename, &uentry,
                    &loadaddr, 0, NULL, NULL);
            boot_info_zephyr.bootstrap_pc = uentry;
        }

        if (kernel_size < 0) {
            fprintf(stderr, "qemu: could not load kernel '%s'\n",
                    kernel_filename);
            exit(1);
        }
    }

    qemu_register_reset(main_cpu_reset, cpu);
}


static void altera_10m50_zephyr_machine_init(MachineClass *mc)
{
    mc->desc = "Altera 10m50 for Zephyr.";
    mc->init = altera_10m50_zephyr_init;
    mc->is_default = 0;
}


DEFINE_MACHINE("altera_10m50_zephyr", altera_10m50_zephyr_machine_init)

