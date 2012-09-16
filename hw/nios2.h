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

#include "sysbus.h"

qemu_irq *nios2_pic_init_cpu(CPUNios2State *env);

static inline DeviceState *
altera_vic_create(target_phys_addr_t base, qemu_irq irq, int kind_of_intr)
{
    DeviceState *dev;

    dev = qdev_create(NULL, "altera,vic");
    qdev_init_nofail(dev);
    sysbus_mmio_map(sysbus_from_qdev(dev), 0, base);
    sysbus_connect_irq(sysbus_from_qdev(dev), 0, irq);
    return dev;
}

static inline DeviceState *
altera_iic_create(Nios2CPU *cpu, qemu_irq irq, int kind_of_intr)
{
    DeviceState *dev;

    dev = qdev_create(NULL, "altera,iic");
    qdev_prop_set_ptr(dev, "cpu", cpu);
    qdev_init_nofail(dev);
    sysbus_connect_irq(sysbus_from_qdev(dev), 0, irq);
    return dev;
}

#endif /* _NIOS2_H_INCLUDED_ */
