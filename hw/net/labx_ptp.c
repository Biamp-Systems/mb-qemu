
/*
 * QEMU model of the LabX PTP.
 *
 * Copyright (c) 2010 Lab X Technologies, LLC
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

#define min_bits(i) (32 - clz32((i)))
#define RAM_INDEX(addr, size) (((addr)>>2)&((1<<min_bits((size)-1))-1))

#define PTP_MAX_PACKETS      8
#define PTP_MAX_PACKET_BYTES 256
#define PTP_RAM_BYTES        (PTP_MAX_PACKETS*PTP_MAX_PACKET_BYTES)
#define PTP_HOST_RAM_WORDS   (PTP_RAM_BYTES/4)

#define TYPE_LABX_PTP "labx.ptp"
#define LABX_PTP(obj) \
    OBJECT_CHECK(LabXPTP, (obj), TYPE_LABX_PTP)

typedef struct LabXPTP {
    SysBusDevice busdev;
    qemu_irq irq;

    MemoryRegion  mmio_ptp;
    MemoryRegion  mmio_tx;
    MemoryRegion  mmio_rx;

    /* Device Configuration */
    uint32_t baseAddress;

    /* Values set by drivers */

    /* Tx buffers */
    uint32_t *txRam;

    /* Rx buffers */
    uint32_t *rxRam;
} LabXPTP;

/*
 * PTP registers
 */
static uint64_t ptp_regs_read(void *opaque, hwaddr addr,
                              unsigned int size)
{
    /*LabXPTP *p = opaque; */

    uint32_t retval = 0;

    switch ((addr>>2) & 0x0F) {
    case 0x00: /* rx */
        break;

    case 0x01: /* tx */
        break;

    case 0x02: /* irq mask */
        break;

    case 0x03: /* irq flags */
        break;

    case 0x04: /* rtc increment */
        break;

    case 0x05: /* seconds high */
        break;

    case 0x06: /* seconds low */
        break;

    case 0x07: /* nanoseconds */
        break;

    case 0x08: /* timer */
        break;

    case 0x09: /* local seconds high */
        break;

    case 0x0A: /* local seconds low */
        break;

    case 0x0B: /* local nanoseconds */
        break;

    case 0x0F: /* revision */
        retval = 0x00000111; /* Report 1 port, revision 1.1 */
        break;

    default:
        printf("labx-ptp: Read of unknown register %"HWADDR_PRIX"\n", addr);
        break;
    }

    return retval;
}

static void ptp_regs_write(void *opaque, hwaddr addr,
                           uint64_t val64, unsigned int size)
{
    /*LabXPTP *p = opaque; */
    uint32_t value = val64;

    switch ((addr>>2) & 0x0F) {
    case 0x00: /* rx */
        break;

    case 0x01: /* tx */
        break;

    case 0x02: /* irq mask */
        break;

    case 0x03: /* irq flags */
        break;

    case 0x04: /* rtc increment */
        break;

    case 0x05: /* seconds high */
        break;

    case 0x06: /* seconds low */
        break;

    case 0x07: /* nanoseconds */
        break;

    case 0x08: /* timer */
        break;

    case 0x09: /* local seconds high */
        break;

    case 0x0A: /* local seconds low */
        break;

    case 0x0B: /* local nanoseconds */
        break;

    case 0x0F: /* revision */
        break;

    default:
        printf("labx-ptp: Write of unknown register %"HWADDR_PRIX" = %08X\n",
               addr, value);
        break;
    }
}

static const MemoryRegionOps ptp_regs_ops = {
    .read = ptp_regs_read,
    .write = ptp_regs_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
    .valid = {
        .min_access_size = 4,
        .max_access_size = 4
    }
};


/*
 * Tx Ram
 */
static uint64_t tx_ram_read(void *opaque, hwaddr addr,
                            unsigned int size)
{
    LabXPTP *p = opaque;

    return p->txRam[RAM_INDEX(addr, PTP_HOST_RAM_WORDS)];
}

static void tx_ram_write(void *opaque, hwaddr addr,
                         uint64_t val64, unsigned int size)
{
    LabXPTP *p = opaque;
    uint32_t value = val64;

    p->txRam[RAM_INDEX(addr, PTP_HOST_RAM_WORDS)] = value;
}

static const MemoryRegionOps tx_ram_ops = {
    .read = tx_ram_read,
    .write = tx_ram_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
    .valid = {
        .min_access_size = 4,
        .max_access_size = 4
    }
};


/*
 * Rx Ram
 */
static uint64_t rx_ram_read(void *opaque, hwaddr addr,
                            unsigned int size)
{
    LabXPTP *p = opaque;

    return p->rxRam[RAM_INDEX(addr, PTP_HOST_RAM_WORDS)];
}

static void rx_ram_write(void *opaque, hwaddr addr,
                         uint64_t val64, unsigned int size)
{
    LabXPTP *p = opaque;
    uint32_t value = val64;

    p->rxRam[RAM_INDEX(addr, PTP_HOST_RAM_WORDS)] = value;
}

static const MemoryRegionOps rx_ram_ops = {
    .read = rx_ram_read,
    .write = rx_ram_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
    .valid = {
        .min_access_size = 4,
        .max_access_size = 4
    }
};


static int labx_ptp_init(SysBusDevice *dev)
{
    LabXPTP *p = LABX_PTP(dev);

    /* Initialize defaults */
    p->txRam = g_malloc0(PTP_RAM_BYTES);
    p->rxRam = g_malloc0(PTP_RAM_BYTES);

    /* Set up memory regions */
    memory_region_init_io(&p->mmio_ptp, OBJECT(p), &ptp_regs_ops, p, "labx.ptp-regs",
                          0x100 * 4);
    memory_region_init_io(&p->mmio_tx,  OBJECT(p), &tx_ram_ops,   p, "labx.ptp-tx",
                          PTP_RAM_BYTES);
    memory_region_init_io(&p->mmio_rx,  OBJECT(p), &rx_ram_ops,   p, "labx.ptp-rx",
                          PTP_RAM_BYTES);

    sysbus_init_mmio(dev, &p->mmio_ptp);
    sysbus_init_mmio(dev, &p->mmio_tx);
    sysbus_init_mmio(dev, &p->mmio_rx);

    /* Offset 0 is automatically mapped. Map the other regions */
    sysbus_mmio_map(dev, 1, p->baseAddress + (1 << min_bits(PTP_RAM_BYTES-1)));
    sysbus_mmio_map(dev, 2, p->baseAddress + (2 << min_bits(PTP_RAM_BYTES-1)));

    sysbus_init_irq(dev, &p->irq);

    return 0;
}

static Property labx_ptp_properties[] = {
    DEFINE_PROP_UINT32("reg", LabXPTP, baseAddress, 0),
    DEFINE_PROP_END_OF_LIST(),
};

static void labx_ptp_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    SysBusDeviceClass *k = SYS_BUS_DEVICE_CLASS(klass);

    k->init = labx_ptp_init;
    dc->props = labx_ptp_properties;
}

static const TypeInfo labx_ptp_info = {
    .name          = TYPE_LABX_PTP,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(LabXPTP),
    .class_init    = labx_ptp_class_init,
};

static const TypeInfo labx_ptp_info2 = {
    .name          = "xlnx.labx-ptp",
    .parent        = TYPE_LABX_PTP,
    .instance_size = sizeof(LabXPTP),
};

static void labx_ptp_register(void)
{
    type_register_static(&labx_ptp_info);
    type_register_static(&labx_ptp_info2);
}

type_init(labx_ptp_register)
