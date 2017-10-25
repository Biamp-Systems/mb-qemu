
/*
 * QEMU model of the LabX Redundancy Switch
 *
 * Copyright (c) 2012 Lab X Technologies, LLC
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
#include "sysemu/sysemu.h"

#define TYPE_LABX_REDUNDANCY_SWITCH "labx.redundancy-switch"
#define LABX_REDUNDANCY_SWITCH(obj) \
    OBJECT_CHECK(LabXRedundancySwitch, (obj), TYPE_LABX_REDUNDANCY_SWITCH)

typedef struct LabXRedundancySwitch {
    SysBusDevice busdev;

    MemoryRegion  mmio_switch_regs;

    /* IRQ */
    qemu_irq irq;

    /* Device Configuration */
    uint32_t baseAddress;

    /* Values set by drivers */

} LabXRedundancySwitch;

/*
 * Redundancy switch registers
 */
static uint64_t switch_regs_read(void *opaque, hwaddr addr,
                                 unsigned int size)
{
    /*LabXRedundancySwitch *p = opaque;*/

    uint32_t retval = 0;

    switch ((addr>>2) & 0x0F) {
    case 0x00: /* control */
        break;

    case 0x02: /* irq flags */
        break;

    case 0x03: /* irq mask */
        break;

    case 0x04: /* stream status 0 */
        break;

    case 0x05: /* stream status 1 */
        break;

    case 0x06: /* stream status 2 */
        break;

    case 0x07: /* stream status 3 */
        break;

    case 0x0F: /* revision */
        retval = 0x00000010;
        break;

    default:
        printf("labx-redundancy-switch: Read of unknown register %"HWADDR_PRIX"\n", addr);
        break;
    }

    return retval;
}

static void switch_regs_write(void *opaque, hwaddr addr,
                              uint64_t val64, unsigned int size)
{
    /*LabXRedundancySwitch *p = opaque; */
    uint32_t value = val64;

    switch ((addr>>2) & 0x0F) {
    case 0x00: /* control */
        break;

    case 0x02: /* irq flags */
        break;

    case 0x03: /* irq mask */
        break;

    case 0x04: /* stream control */
        break;

    case 0x0F: /* revision */
        break;

    default:
        printf("labx-redundancy-switch: Write of unknown register"
               "%"HWADDR_PRIX" = %08X\n", addr, value);
        break;
    }
}

static const MemoryRegionOps switch_regs_ops = {
    .read = switch_regs_read,
    .write = switch_regs_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
    .valid = {
        .min_access_size = 4,
        .max_access_size = 4
    }
};


static int labx_redundancy_switch_init(SysBusDevice *dev)
{
    LabXRedundancySwitch *p = LABX_REDUNDANCY_SWITCH(dev);

    /* Initialize defaults */

    /* Set up the IRQ */
    sysbus_init_irq(dev, &p->irq);

    /* Set up memory regions */
    memory_region_init_io(&p->mmio_switch_regs,          OBJECT(p), &switch_regs_ops, p,
                          "labx,redundancy-switch-regs", 0x10 * 4);

    sysbus_init_mmio(dev, &p->mmio_switch_regs);

    /* Offset 0 is automatically mapped. */

    return 0;
}

static Property labx_redundancy_switch_properties[] = {
    DEFINE_PROP_END_OF_LIST(),
};

static void labx_redundancy_switch_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    SysBusDeviceClass *k = SYS_BUS_DEVICE_CLASS(klass);

    k->init = labx_redundancy_switch_init;
    dc->props = labx_redundancy_switch_properties;
}

static const TypeInfo labx_redundancy_switch_info = {
    .name          = TYPE_LABX_REDUNDANCY_SWITCH,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(LabXRedundancySwitch),
    .class_init    = labx_redundancy_switch_class_init,
};

static const TypeInfo labx_redundancy_switch_info2 = {
    .name          = "xlnx.labx-redundancy-switch",
    .parent        = TYPE_LABX_REDUNDANCY_SWITCH,
    .instance_size = sizeof(LabXRedundancySwitch),
    .class_init    = labx_redundancy_switch_class_init,
};

static void labx_redundancy_switch_register(void)
{
    type_register_static(&labx_redundancy_switch_info);
    type_register_static(&labx_redundancy_switch_info2);
}

type_init(labx_redundancy_switch_register)
