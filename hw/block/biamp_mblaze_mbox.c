
/*
 * QEMU model of the Biamp Microblaze Mailbox core.
 *
 * Copyright (c) 2017 Biamp Systems
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
#include "chardev/char-fe.h"
#include "qapi/error.h"
#include "hw/sysbus.h"
#include "sysemu/sysemu.h"

#define MBLAZE_MBOX_BYTES 4096
#define MBLAZE_MBOX_ADDR_MASK 0x3FF
#define TYPE_BIAMP_MBMBOX "biamp.mblaze-mbox"
#define BIAMP_MBMBOX(obj) OBJECT_CHECK(BiampMbMbox, (obj), TYPE_BIAMP_MBMBOX)


typedef enum {EVENT_READY, WRITE_BUFFER} mblaze_opcode;

typedef struct BiampMbMbox{
    SysBusDevice busdev;
    qemu_irq responseRdyIrq;
    qemu_irq eventRdyIrq;

    CharBackend chr_fifo;

    MemoryRegion  mmbox;

    /* Mailbox buffer */
    uint32_t * mailboxBuffer;
    uint32_t mailboxIndex;


} BiampMbMbox;


static uint64_t mailbox_read(void *opaque, hwaddr addr,
                                  unsigned int size)
{
    BiampMbMbox *p = opaque;    

    return p->mailboxBuffer[(addr >> 2) & MBLAZE_MBOX_ADDR_MASK];
}


static void mailbox_write(void *opaque, hwaddr addr,
                               uint64_t val64, unsigned int size)
{
    BiampMbMbox *p = opaque;
    uint32_t val32 = (uint32_t)val64;

    p->mailboxBuffer[(addr >> 2) & MBLAZE_MBOX_ADDR_MASK] = val32;
}

static const MemoryRegionOps mailbox_mem_ops = {
    .read = mailbox_read,
    .write = mailbox_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
    .impl = {
        .min_access_size = 4,
        .max_access_size = 4,
    },
};

/* GPIO callback function */
static void message_ready_irq(void *opaque, int irq, int level){
    BiampMbMbox *p = opaque;
    const uint8_t * mailboxBuffer8 = (uint8_t *)(p->mailboxBuffer);
    if(level){
        qemu_chr_fe_write(&(p->chr_fifo), mailboxBuffer8, MBLAZE_MBOX_BYTES);
    }
}

/* Called when host writes to the character pipe
 * First byte is an op code, either EVENT_READY(0) or WRITE_BUFFER(1).
 * When EVENT_READY, the eventRdyIrq interrupt is raised.
 * When WRITE_BUFFER, the next MBLAZE_MBOX_BYTES are written to the mailbox buffer,
 * followed by the responseRdyIrq once all the bytes are recieved/written.
 */
static void host_to_mbox(void *opaque, const uint8_t *buf, int size){
    BiampMbMbox *p = opaque;

    size = size % (MBLAZE_MBOX_BYTES + 1);
    if(p->mailboxIndex == 0){
        switch((mblaze_opcode)buf[0]){
        case(EVENT_READY) :
            qemu_irq_raise(p->eventRdyIrq);
            break;
        case(WRITE_BUFFER):
            p->mailboxIndex = 1;
            break;
        default:
            break;
        }
    }
    else{
        for(int i = p->mailboxIndex; i <= size + p->mailboxIndex && i < MBLAZE_MBOX_BYTES; i++){
            ((uint8_t*)(p->mailboxBuffer))[i-1] = buf[i];
        }
        p->mailboxIndex += size;
        if(p->mailboxIndex > MBLAZE_MBOX_BYTES){
            qemu_irq_raise(p->responseRdyIrq);
            p->mailboxIndex = 0;
        }
    }
}

static int mb_can_rx(void *opaque){
    return true;
}

static void chr_fifo_event(void *opaque, int event){
    BiampMbMbox *p = opaque;
    qemu_chr_fe_accept_input(&(p->chr_fifo));  
}

static int biamp_mblaze_mbox_init(SysBusDevice *dev)
{
    BiampMbMbox *p = BIAMP_MBMBOX(dev);
    
    /* Initialize the mailbox buffer */
    p->mailboxBuffer = g_malloc0(MBLAZE_MBOX_BYTES);

    /* Set up memory regions */
    memory_region_init_io(&p->mmbox, OBJECT(p), &mailbox_mem_ops, p,
                          "biamp-mblze-mailbox-mem", MBLAZE_MBOX_BYTES);
    sysbus_init_mmio(dev, &p->mmbox);

    /* Initialize the IRQs */
    sysbus_init_irq(SYS_BUS_DEVICE(dev), &p->eventRdyIrq);
    sysbus_init_irq(SYS_BUS_DEVICE(dev), &p->responseRdyIrq);
    qdev_init_gpio_in(DEVICE(dev), message_ready_irq, 1); 

    /* Initialize the chardev front ends */
    p->mailboxIndex = 0;
    qemu_chr_fe_init(&(p->chr_fifo), qemu_chr_find("mbmbx") , &error_abort);
    qemu_chr_fe_set_handlers(&(p->chr_fifo), mb_can_rx, host_to_mbox, chr_fifo_event, NULL, p, NULL, true);

    return 0;
}

static Property biamp_mbmbox_properties[] = {
    DEFINE_PROP_CHR("chardev0", BiampMbMbox, chr_fifo),
    DEFINE_PROP_END_OF_LIST(),
};

static void biamp_mbmbox_class_init(ObjectClass *klass, void *data){
    DeviceClass *dc = DEVICE_CLASS(klass);
    SysBusDeviceClass *k = SYS_BUS_DEVICE_CLASS(klass);
    k->init = biamp_mblaze_mbox_init;
    dc->props = biamp_mbmbox_properties;
}

static const TypeInfo biamp_mbmbox_info = {
    .name   = TYPE_BIAMP_MBMBOX,
    .parent = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(BiampMbMbox),
    .class_init = biamp_mbmbox_class_init,
};

static void biamp_mbmbox_register(void)
{
    type_register_static(&biamp_mbmbox_info);
}

type_init(biamp_mbmbox_register)
