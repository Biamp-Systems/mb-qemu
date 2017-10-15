/*
 * QEMU Altera Internal Interrupt Controller.
 *
 * Copyright (c) 2012 Chris Wulff <crwulff@gmail.com>
 * Copyright (c) 2016 Intel Corporation.
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
#include "cpu.h"
#include "hw/nios2/nios2_iic.h"
#include "hw/fdt_generic_util.h"

#define TYPE_ALTERA_IIC "altera,iic"
#define ALTERA_IIC(obj) \
    OBJECT_CHECK(AlteraIIC, (obj), TYPE_ALTERA_IIC)

typedef struct AlteraIIC {
    SysBusDevice  parent_obj;
    void         *cpu;
    qemu_irq      parent_irq;
    uint32_t      irqs;
} AlteraIIC;

/*
 * When the internal interrupt controller is implemented, a peripheral
 * device can request a hardware interrupt by asserting one of the Nios II
 * processor’s 32 interrupt-request inputs, irq0 through irq31. A hardware
 * interrupt is generated if and only if all three of these conditions are
 * true:
 *  1.    The PIE bit of the status control register is one.
 *  2.    An interrupt-request input, irq n, is asserted.
 *  3.    The corresponding bit n of the ienable control register is one

 * ipending register:
 * A value of one in bit n means that the corresponding irq n input is
 * asserted and enabled in the ienable register.
 */

static void update_irq(AlteraIIC *pv)
{
    CPUNios2State *env = &((Nios2CPU *)(pv->cpu))->env;

    if ((env->regs[CR_STATUS] & CR_STATUS_PIE) == 0) {
        qemu_irq_lower(pv->parent_irq);
        return;
    }

    if (env->regs[CR_IPENDING]) {
        qemu_irq_raise(pv->parent_irq);
    } else {
        qemu_irq_lower(pv->parent_irq);
    }
}

/* Emulate the CR_IPENDING register */
 static void irq_handler(void *opaque, int irq, int level)
 {
    AlteraIIC *s = opaque;
    CPUNios2State *env = &((Nios2CPU *)(s->cpu))->env;

    /* Keep track of IRQ lines states */
    s->irqs &= ~(1 << irq);
    s->irqs |= level << irq;
    env->regs[CR_IPENDING] = env->regs[CR_IENABLE] & s->irqs;
    update_irq(s);
}

/* This routine must be called when CR_IENABLE is modified */
void nios2_iic_update_cr_ienable(DeviceState *d)
{
    /* Modify the IPENDING register */
    AlteraIIC *s = ALTERA_IIC(d);
    CPUNios2State *env = &((Nios2CPU *)(s->cpu))->env;
    env->regs[CR_IPENDING] = env->regs[CR_IENABLE] & s->irqs;
    update_irq(s);
}

/* This routine must be called when CR_STATUS is modified,
 * in particular the bit CR_STATUS_PIE
 */
void nios2_iic_update_cr_status(DeviceState *d)
{
    AlteraIIC *s = ALTERA_IIC(d);
    update_irq(s);
}

void nios2_iic_create(Nios2CPU *cpu)
{
    DeviceState *dev;

    dev = qdev_create(NULL, "altera,iic");
    qdev_prop_set_ptr(dev, "cpu", cpu);
    qdev_init_nofail(dev);
    cpu->env.pic_state = dev;
    qdev_connect_gpio_out_named(dev, "irq", 0,
                                qdev_get_gpio_in(DEVICE(first_cpu), 0));
}

static void altera_iic_init(Object *obj)
{
    AlteraIIC *pv = ALTERA_IIC(obj);

    qdev_init_gpio_in(DEVICE(pv), irq_handler, 32);
    qdev_init_gpio_out_named(DEVICE(obj), &pv->parent_irq, "irq", 1);
}

static int altera_iic_fdt_get_irq(FDTGenericIntc *obj, qemu_irq *irqs,
                                  uint32_t *cells, int ncells, int max,
                                  Error **errp)
{
    uint32_t idx;

    if (ncells != 1) {
        error_setg(errp, "IIC requires 1 interrupt cells: %d given",
                   ncells);
        return 0;
    }
    idx = cells[0];

    if (idx >= 32) {
        error_setg(errp, "IIC only supports 32 interrupts: index %"
                   PRId32 " requested", idx);
        return 0;
    }

    (*irqs) = qdev_get_gpio_in(DEVICE(obj), idx);
    return 1;
};

static Property altera_iic_properties[] = {
    DEFINE_PROP_PTR("cpu", AlteraIIC, cpu),
    DEFINE_PROP_END_OF_LIST(),
};

static void altera_iic_fdt_auto_parent(FDTGenericIntc *obj, Error **errp)
{
    qdev_connect_gpio_out_named(DEVICE(obj), "irq", 0,
                                qdev_get_gpio_in(DEVICE(first_cpu), 0));
}

static void altera_iic_realize(DeviceState *dev, Error **errp)
{
    struct AlteraIIC *pv = ALTERA_IIC(dev);

    if (!pv->cpu) {
        error_setg(errp, "altera,iic: CPU not connected");
        return;
    }
}

static void altera_iic_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    FDTGenericIntcClass *fgic = FDT_GENERIC_INTC_CLASS(klass);

    dc->props = altera_iic_properties;
    /* Reason: pointer property "cpu" */
    dc->cannot_instantiate_with_device_add_yet = true;
    dc->realize = altera_iic_realize;
    fgic->get_irq = altera_iic_fdt_get_irq;
    fgic->auto_parent = altera_iic_fdt_auto_parent;
}

static TypeInfo altera_iic_info = {
    .name          = "altera,iic",
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(AlteraIIC),
    .instance_init = altera_iic_init,
    .class_init    = altera_iic_class_init,
    .interfaces = (InterfaceInfo[]) {
        { TYPE_FDT_GENERIC_INTC },
        { }
    },
};

static void altera_iic_register(void)
{
    type_register_static(&altera_iic_info);
}

type_init(altera_iic_register)
