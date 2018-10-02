
/*
 * QEMU model of the Biamp stream shaper
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

#define TYPE_STREAM_SHAPER "xlnx,stream-shaper-1.0"
#define STREAM_SHAPER(obj) \
    OBJECT_CHECK(StreamShaper, (obj), TYPE_STREAM_SHAPER)

typedef struct StreamShaper {
    SysBusDevice busdev;

    MemoryRegion  mmio_stream_shaper;

		uint32_t dwidth;
		uint32_t awidth;
		uint32_t slave_native_awidth;
		uint32_t baseAddress;

    /* Values set by drivers */
    uint32_t idleSlope;
    uint32_t sendSlope;
    uint32_t bypass;
} StreamShaper;

/*
 * StreamShaper registers
 */
static uint64_t stream_shaper_regs_read(void *opaque, hwaddr addr,
                                       unsigned int size)
{
		fprintf(stderr, "%s: %d\n",__func__,__LINE__);
    StreamShaper *p = opaque;

    uint32_t retval = 0;

    switch ((addr>>2) & 0xFF) {

    default:
        fprintf(stderr,"biamp-stream-shaper: Read of unknown register %"HWADDR_PRIX"\n", addr);
        break;
    }

    return retval;
}

static void stream_shaper_regs_write(void *opaque, hwaddr addr,
                                    uint64_t val64, unsigned int size)
{
    /*StreamShaper *p = opaque; */
    uint32_t value = val64;

		fprintf(stderr,"%s: %d\n",__func__,__LINE__);
    switch ((addr>>2) & 0xFF) {

    default:
        fprintf(stderr,"biamp-stream-shaper: Write of unknown register "
               "%"HWADDR_PRIX" = %08X\n", addr, value);
        break;
    }
}

static const MemoryRegionOps stream_shaper_regs_ops = {
    .read = stream_shaper_regs_read,
    .write = stream_shaper_regs_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
    .valid = {
        .min_access_size = 4,
        .max_access_size = 4
    }
};




static int biamp_stream_shaper_init(SysBusDevice *dev)
{
		fprintf(stderr,"%s: %d\n",__func__,__LINE__);
    StreamShaper *p = STREAM_SHAPER(dev);


    /* Initialize defaults */
    p->idleSlope = 0;
		p->sendSlope = 0;
		p->bypass    = 0;

		fprintf(stderr,"%s:%d\n",__FILE__,__LINE__);
#if 0
		/* May not need, stream_shaper has not interrupts */
    /* Set up the IRQ */
    sysbus_init_irq(dev, &p->irq);

#endif

		fprintf(stderr,"%s:%d\n",__FILE__,__LINE__);
    /* Set up memory regions */
    memory_region_init_io(&p->mmio_stream_shaper, OBJECT(p), &stream_shaper_regs_ops, p, "xlnx.stream-shaper-regs", 0x10000);
    sysbus_init_mmio(dev, &p->mmio_stream_shaper);
		fprintf(stderr,"%s:%d\n",__FILE__,__LINE__);
    return 0;
}

static Property biamp_stream_shaper_properties[] = {
    DEFINE_PROP_UINT32("reg",                 StreamShaper, baseAddress,    0),
    DEFINE_PROP_UINT32("slave-awidth",        StreamShaper, awidth,      0x20),
    DEFINE_PROP_UINT32("slave-dwidth",        StreamShaper, dwidth,      0x20),
    DEFINE_PROP_UINT32("slave-native-dwidth", StreamShaper, dwidth,      0x20),
    DEFINE_PROP_END_OF_LIST(),
};

static void biamp_stream_shaper_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    SysBusDeviceClass *k = SYS_BUS_DEVICE_CLASS(klass);

		fprintf(stderr,"%s:%d\n",__FILE__,__LINE__);
    k->init = biamp_stream_shaper_init;
    dc->props = biamp_stream_shaper_properties;
}

static const TypeInfo biamp_stream_shaper_info = {
    .name          = TYPE_STREAM_SHAPER,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(StreamShaper),
    .class_init    = biamp_stream_shaper_class_init,
};

static void biamp_stream_shaper_register(void)
{
		fprintf(stderr,"%s:%d\n",__FILE__,__LINE__);
    type_register_static(&biamp_stream_shaper_info);
}

type_init(biamp_stream_shaper_register)
