/*
 * QEMU model of the Altera timer.
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
#include "qemu-common.h"
#include "qapi/error.h"

#include "hw/sysbus.h"
#include "sysemu/sysemu.h"
#include "hw/ptimer.h"
#include "hw/nios2/altera_timer.h"

#define R_STATUS      0
#define R_CONTROL     1
#define R_PERIODL     2
#define R_PERIODH     3
#define R_SNAPL       4
#define R_SNAPH       5
#define R_MAX         6

#define STATUS_TO     0x0001
#define STATUS_RUN    0x0002

#define CONTROL_ITO   0x0001
#define CONTROL_CONT  0x0002
#define CONTROL_START 0x0004
#define CONTROL_STOP  0x0008

#define TYPE_ALTERA_TIMER "ALTR.timer"
#define ALTERA_TIMER(obj) \
    OBJECT_CHECK(AlteraTimer, (obj), TYPE_ALTERA_TIMER)

typedef struct AlteraTimer {
    SysBusDevice  busdev;
    MemoryRegion  mmio;
    qemu_irq      irq;
    uint32_t      freq_hz;
    QEMUBH       *bh;
    ptimer_state *ptimer;
    uint32_t      regs[R_MAX];
} AlteraTimer;

static inline int timer_irq_state(AlteraTimer *t)
{
    return ((t->regs[R_STATUS] & STATUS_TO) &&
            (t->regs[R_CONTROL] & CONTROL_ITO)) ? 1 : 0;
}

static uint64_t timer_read(void *opaque, hwaddr addr,
                           unsigned int size)
{
    AlteraTimer *t = opaque;
    uint64_t r = 0;

    addr >>= 2;
    addr &= 0x7;
    switch (addr) {
    case R_CONTROL:
        r = t->regs[R_CONTROL] & (CONTROL_ITO | CONTROL_CONT);
        break;

    default:
        if (addr < ARRAY_SIZE(t->regs)) {
            r = t->regs[addr];
        }
        break;
    }

    return r;
}

static void timer_write(void *opaque, hwaddr addr,
                        uint64_t val64, unsigned int size)
{
    AlteraTimer *t = opaque;
    uint64_t tvalue;
    uint32_t value = val64;
    uint32_t count = 0;
    int irqState = timer_irq_state(t);

    addr >>= 2;
    addr &= 0x7;
    switch (addr) {
    case R_STATUS:
        /* Writing zero clears the timeout */
        t->regs[R_STATUS] &= ~STATUS_TO;
        break;

    case R_CONTROL:
        t->regs[R_CONTROL] = value & (CONTROL_ITO | CONTROL_CONT);
        if ((value & CONTROL_START) &&
            !(t->regs[R_STATUS] & STATUS_RUN)) {
            ptimer_run(t->ptimer, 1);
            t->regs[R_STATUS] |= STATUS_RUN;
        }
        if ((value & CONTROL_STOP) && (t->regs[R_STATUS] & STATUS_RUN)) {
            ptimer_stop(t->ptimer);
            t->regs[R_STATUS] &= ~STATUS_RUN;
        }
        break;

    case R_PERIODL:
    case R_PERIODH:
        t->regs[addr] = value & 0xFFFF;
        if (t->regs[R_STATUS] & STATUS_RUN) {
            ptimer_stop(t->ptimer);
            t->regs[R_STATUS] &= ~STATUS_RUN;
        }
        tvalue = (t->regs[R_PERIODH] << 16) | t->regs[R_PERIODL];
        ptimer_set_limit(t->ptimer, tvalue + 1, 1);
        break;

    case R_SNAPL:
    case R_SNAPH:
        count = ptimer_get_count(t->ptimer);
        t->regs[R_SNAPL] = count & 0xFFFF;
        t->regs[R_SNAPH] = (count >> 16) & 0xFFFF;
        break;

    default:
        break;
    }

    if (irqState != timer_irq_state(t)) {
        qemu_set_irq(t->irq, timer_irq_state(t));
    }
}

static const MemoryRegionOps timer_ops = {
    .read = timer_read,
    .write = timer_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
    .valid = {
        .min_access_size = 1,
        .max_access_size = 4
    }
};

static void timer_hit(void *opaque)
{
    AlteraTimer *t = opaque;
    const uint64_t tvalue = (t->regs[R_PERIODH] << 16) | t->regs[R_PERIODL];

    t->regs[R_STATUS] |= STATUS_TO;

    ptimer_stop(t->ptimer);
    ptimer_set_limit(t->ptimer, tvalue + 1, 1);

    if (t->regs[R_CONTROL] & CONTROL_CONT) {
        ptimer_run(t->ptimer, 1);
    } else {
        t->regs[R_STATUS] &= ~STATUS_RUN;
    }

    qemu_set_irq(t->irq, timer_irq_state(t));
}

static void altera_timer_realize(DeviceState *dev, Error **errp)
{
    AlteraTimer *t = ALTERA_TIMER(dev);
    SysBusDevice *sbd = SYS_BUS_DEVICE(dev);

    assert(t->freq_hz != 0);

    t->bh = qemu_bh_new(timer_hit, t);
    t->ptimer = ptimer_init(t->bh);
    ptimer_set_freq(t->ptimer, t->freq_hz);
    ptimer_set_limit(t->ptimer, 0xffffffff, 1);

    memory_region_init_io(&t->mmio, OBJECT(t), &timer_ops, t,
                          TYPE_ALTERA_TIMER, R_MAX * sizeof(uint32_t));
    sysbus_init_mmio(sbd, &t->mmio);
}

static void altera_timer_init(Object *obj)
{
    AlteraTimer *t = ALTERA_TIMER(obj);
    SysBusDevice *sbd = SYS_BUS_DEVICE(obj);

    sysbus_init_irq(sbd, &t->irq);
}

void altera_timer_create(const hwaddr addr, qemu_irq irq, uint32_t frequency)
{
    DeviceState *dev;
    SysBusDevice *bus;

    dev = qdev_create(NULL, TYPE_ALTERA_TIMER);

    qdev_prop_set_uint32(dev, "clock-frequency", frequency);
    bus = SYS_BUS_DEVICE(dev);
    qdev_init_nofail(dev);

    if (addr != (hwaddr)-1) {
        sysbus_mmio_map(bus, 0, addr);
    }

    sysbus_connect_irq(bus, 0, irq);
}

static Property altera_timer_properties[] = {
    DEFINE_PROP_UINT32("clock-frequency", AlteraTimer, freq_hz, 0),
    DEFINE_PROP_END_OF_LIST(),
};

static void altera_timer_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->realize = altera_timer_realize;
    dc->props = altera_timer_properties;
}

static const TypeInfo altera_timer_info = {
    .name          = TYPE_ALTERA_TIMER,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(AlteraTimer),
    .instance_init = altera_timer_init,
    .class_init    = altera_timer_class_init,
};

static void altera_timer_register(void)
{
    type_register_static(&altera_timer_info);
}

type_init(altera_timer_register)
