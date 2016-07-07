/*
 * Altera Nios II CPU interrupt controllers
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

#ifndef _NIOS2_H_INCLUDED_
#define _NIOS2_H_INCLUDED_

#include "hw/sysbus.h"
#include "cpu.h"

qemu_irq *nios2_pic_init_cpu(Nios2CPU *cpu);

static inline DeviceState *
altera_vic_create(hwaddr base, qemu_irq irq, int kind_of_intr)
{
    DeviceState *dev;

    dev = qdev_create(NULL, "altera,vic");
    qdev_init_nofail(dev);
    sysbus_mmio_map(SYS_BUS_DEVICE(dev), 0, base);
    sysbus_connect_irq(SYS_BUS_DEVICE(dev), 0, irq);
    return dev;
}

#endif /* _NIOS2_H_INCLUDED_ */
