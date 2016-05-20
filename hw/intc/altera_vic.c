/*
 * QEMU Altera Vectored Interrupt Controller.
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

#define R_INT_CONFIG_0      0
#define R_INT_ENABLE       32
#define R_INT_ENABLE_SET   33
#define R_INT_ENABLE_CLR   34
#define R_INT_PENDING      35
#define R_INT_RAW_STATUS   36
#define R_SW_INTERRUPT     37
#define R_SW_INTERRUPT_SET 38
#define R_SW_INTERRUPT_CLR 39
#define R_VIC_CONFIG       40
#define R_VIC_STATUS       41
#define R_VEC_TBL_BASE     42
#define R_VEC_TBL_ADDR     43
#define R_MAX              44

#define TYPE_ALTERA_VIC "altera,vic"
#define ALTERA_VIC(obj) \
    OBJECT_CHECK(AlteraVIC, (obj), TYPE_ALTERA_VIC)

typedef struct AlteraVIC {
    SysBusDevice busdev;
    MemoryRegion mmio;
    qemu_irq parent_irq;

    /* Runtime control registers.  */
    uint32_t regs[R_MAX];
} AlteraVIC;

static void update_irq(AlteraVIC *pv)
{
    uint32_t i;
    pv->regs[R_INT_PENDING] = (pv->regs[R_INT_RAW_STATUS] |
                               pv->regs[R_SW_INTERRUPT]) &
                              pv->regs[R_INT_ENABLE];

    for (i = 0; i < 32; i++) {
        if (pv->regs[R_INT_PENDING] & (1 << i)) {
            break;
        }
    }
    if (i == 32) {
        pv->regs[R_VEC_TBL_ADDR] = 0;
        pv->regs[R_VIC_STATUS] = 0;
        qemu_irq_lower(pv->parent_irq);
    } else {
        pv->regs[R_VEC_TBL_ADDR] = pv->regs[R_VEC_TBL_BASE] +
                                   i * (4 << (pv->regs[R_VIC_CONFIG] & 7));
        pv->regs[R_VIC_STATUS] = 0x80000000 | i;
        qemu_irq_raise(pv->parent_irq);
    }
}

static uint64_t pic_read(void *opaque, hwaddr addr,
                         unsigned int size)
{
    AlteraVIC *pv = opaque;
    uint32_t r = 0;

    addr >>= 2;
    if (addr < R_MAX) {
        r = pv->regs[addr];
    }

    return r;
}

static void pic_write(void *opaque, hwaddr addr,
                      uint64_t val64, unsigned int size)
{
    AlteraVIC *pv = opaque;
    uint32_t value = val64;

    addr >>= 2;
    if (addr < R_INT_ENABLE) {
        /* R_INT_CONFIG_XX */
        pv->regs[addr] = value & 0x00001FFF;
    } else {
        switch (addr) {
        case R_INT_PENDING:
        case R_INT_RAW_STATUS:
        case R_VIC_STATUS:
        case R_VEC_TBL_ADDR:
            /* read only */
            break;

        case R_INT_ENABLE_SET:
            pv->regs[R_INT_ENABLE] |= value;
            break;

        case R_SW_INTERRUPT_SET:
            pv->regs[R_SW_INTERRUPT] |= value;
            break;

        case R_INT_ENABLE_CLR:
            pv->regs[R_INT_ENABLE] &= ~value;
            break;

        case R_SW_INTERRUPT_CLR:
            pv->regs[R_SW_INTERRUPT] &= ~value;
            break;

        case R_VIC_CONFIG:
            pv->regs[addr] = value & 0x0000000F;
            break;

        default:
            if (addr < ARRAY_SIZE(pv->regs)) {
                pv->regs[addr] = value;
            }
            break;
        }
    }
    update_irq(pv);
}

static const MemoryRegionOps pic_ops = {
    .read = pic_read,
    .write = pic_write,
    .endianness = DEVICE_LITTLE_ENDIAN,
    .valid = {
        .min_access_size = 4,
        .max_access_size = 4
    }
};

static void irq_handler(void *opaque, int irq, int level)
{
    AlteraVIC *pv = opaque;

    pv->regs[R_INT_RAW_STATUS] &= ~(1 << irq);
    pv->regs[R_INT_RAW_STATUS] |= level << irq;

    update_irq(pv);
}

static int altera_vic_init(SysBusDevice *dev)
{
    AlteraVIC *pv = ALTERA_VIC(dev);

    qdev_init_gpio_in(DEVICE(pv), irq_handler, 32);
    sysbus_init_irq(dev, &pv->parent_irq);

    memset(pv->regs, 0, sizeof(uint32_t) * R_MAX);
    memory_region_init_io(&pv->mmio, OBJECT(pv), &pic_ops, pv,
                          TYPE_ALTERA_VIC, R_MAX * sizeof(uint32_t));
    sysbus_init_mmio(dev, &pv->mmio);
    return 0;
}

static Property altera_vic_properties[] = {
    DEFINE_PROP_END_OF_LIST(),
};

static void altera_vic_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    SysBusDeviceClass *k = SYS_BUS_DEVICE_CLASS(klass);

    k->init = altera_vic_init;
    dc->props = altera_vic_properties;
}

static const TypeInfo altera_vic_info = {
    .name          = TYPE_ALTERA_VIC,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(AlteraVIC),
    .class_init    = altera_vic_class_init,
};

static void altera_vic_register(void)
{
    type_register_static(&altera_vic_info);
}

type_init(altera_vic_register)
