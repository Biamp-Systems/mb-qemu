
/*
 * QEMU model of the Biamp buffer clear
 *
 * Copyright (c) 2018 Biamp Systems
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
#include "hw/labx_devices.h"

#define min_bits(i) (32 - clz32((i)))
#define RAM_INDEX(addr, size) (((addr)>>2)&((1<<min_bits((size)-1))-1))

#define TYPE_BUFFER_CLEAR "biamp,buffer-clear"
#define BUFFER_CLEAR(obj) \
    OBJECT_CHECK(BufferClear, (obj), TYPE_BUFFER_CLEAR)

typedef struct BufferClear {
    SysBusDevice busdev;

    MemoryRegion  mmio_buffer_clear;
		uint32_t baseAddress;
} BufferClear;

/*
 * BufferClear registers
 */
static uint64_t buffer_clear_regs_read(void *opaque, hwaddr addr,
                                       unsigned int size)
{
    uint32_t retval = 0;

    switch ((addr>>2) & 0xFF) {
			case 0:
			case 1:
			case 2:
			case 3: retval = 1; break;

    default:
        fprintf(stderr,"biamp-buffer-clear: Read of unknown register %"HWADDR_PRIX"\n", addr);
        break;
    }

    return retval;
}

static void buffer_clear_regs_write(void *opaque, hwaddr addr,
                                    uint64_t val64, unsigned int size)
{
    /*BufferClear *p = opaque; */
    uint32_t value = val64;

    switch ((addr>>2) & 0xFF) {

			case 0:
			case 1:
			case 2:
			case 3: break; 

    default:
        fprintf(stderr,"biamp-buffer-clear: Write of unknown register "
               "%"HWADDR_PRIX" = %08X\n", addr, value);
        break;
    }
}

static const MemoryRegionOps buffer_clear_regs_ops = {
    .read = buffer_clear_regs_read,
    .write = buffer_clear_regs_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
    .valid = {
        .min_access_size = 4,
        .max_access_size = 4
    }
};




static int biamp_buffer_clear_init(SysBusDevice *dev)
{
    BufferClear *p = BUFFER_CLEAR(dev);


    /* Set up memory regions */
    memory_region_init_io(&p->mmio_buffer_clear, OBJECT(p), &buffer_clear_regs_ops, p, "biamp.buffer-clear-regs", 0x10000);
    sysbus_init_mmio(dev, &p->mmio_buffer_clear);
    return 0;
}

static Property biamp_buffer_clear_properties[] = {
    DEFINE_PROP_UINT32("reg",                 BufferClear, baseAddress,    0),
    DEFINE_PROP_END_OF_LIST(),
};

static void biamp_buffer_clear_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    SysBusDeviceClass *k = SYS_BUS_DEVICE_CLASS(klass);

    k->init = biamp_buffer_clear_init;
    dc->props = biamp_buffer_clear_properties;
}

static const TypeInfo biamp_buffer_clear_info = {
    .name          = TYPE_BUFFER_CLEAR,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(BufferClear),
    .class_init    = biamp_buffer_clear_class_init,
};

static void biamp_buffer_clear_register(void)
{
    type_register_static(&biamp_buffer_clear_info);
}

type_init(biamp_buffer_clear_register)
