
/*
 * QEMU model of the LabX legacy ethernet core.
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
#include "net/net.h"

#define FIFO_RAM_BYTES 2048
#define LENGTH_FIFO_WORDS 16

#define TYPE_LABX_ETHERNET "labx.ethernet"
#define LABX_ETHERNET(obj) \
    OBJECT_CHECK(LabXEthernet, (obj), TYPE_LABX_ETHERNET)

typedef struct LabXEthernet {
    SysBusDevice busdev;
    qemu_irq hostIrq;
    qemu_irq fifoIrq;
    qemu_irq phyIrq;
    NICState *nic;
    NICConf conf;

    MemoryRegion  mmio_ethernet;
    MemoryRegion  mmio_mac;
    MemoryRegion  mmio_fifo;

    /* Device Configuration */
    uint32_t baseAddress;

    /* Values set by drivers */
    uint32_t hostRegs[0x10];
    uint32_t fifoRegs[0x10];

    /* Tx buffers */
    uint32_t *txBuffer;
    uint32_t  txPushIndex;
    uint32_t  txPopIndex;

    uint32_t *txLengthBuffer;
    uint32_t  txLengthPushIndex;
    uint32_t  txLengthPopIndex;

    /* Rx buffers */
    uint32_t *rxBuffer;
    uint32_t  rxPushIndex;
    uint32_t  rxPopIndex;

    uint32_t *rxLengthBuffer;
    uint32_t  rxLengthPushIndex;
    uint32_t  rxLengthPopIndex;
} LabXEthernet;

/*
 * Legacy ethernet registers
 */
static void update_host_irq(LabXEthernet *p)
{
    if ((p->hostRegs[0x03] & p->hostRegs[2]) != 0) {
        qemu_irq_raise(p->hostIrq);
    } else {
        qemu_irq_lower(p->hostIrq);
    }
}

static void mdio_xfer(LabXEthernet *p, int readWrite,
                      int phyAddr, int regAddr)
{
    printf("MDIO %s: addr=%d, reg=%d\n", (readWrite) ? "READ" : "WRITE",
           phyAddr, regAddr);
    if (readWrite) {
        /* TODO: PHY info */
        p->hostRegs[0x01] = 0x0000FFFF;
    }
    p->hostRegs[0x03] |= 1;
    update_host_irq(p);
}

static uint64_t ethernet_regs_read(void *opaque, hwaddr addr,
                                   unsigned int size)
{
    LabXEthernet *p = opaque;

    uint32_t retval = 0;

    switch ((addr>>2) & 0x0F) {
    case 0x00: /* mdio control */
    case 0x01: /* mdio data */
    case 0x02: /* irq mask */
    case 0x03: /* irq flags */
    case 0x04: /* vlan mask */
    case 0x05: /* filter select */
        retval = p->hostRegs[(addr>>2) & 0x0F];
        break;

    case 0x06: /* filter control */
        retval = 0x20000000;
        break;

    case 0x0F: /* revision */
        retval = 0x00000C13;
        break;

    case 0x07: /* filter load */
        retval = p->hostRegs[(addr>>2) & 0x0F];
        break;

    case 0x08: /* bad packet */
        retval = 0;
        break;

    default:
        printf("labx-ethernet: Read of unknown register %"HWADDR_PRIX"\n", addr);
        break;
    }

    return retval;
}

static void ethernet_regs_write(void *opaque, hwaddr addr,
                                uint64_t val64, unsigned int size)
{
    LabXEthernet *p = opaque;
    uint32_t value = val64;

    switch ((addr>>2) & 0x0F) {
    case 0x00: /* mdio control */
        p->hostRegs[0x00] = (value & 0x000007FF);
        mdio_xfer(p, (value >> 10) & 1, (value >> 5) & 0x1F, value & 0x1F);
        break;

    case 0x01: /* mdio data */
        p->hostRegs[0x01] = (value & 0x0000FFFF);
        break;

    case 0x02: /* irq mask */
        p->hostRegs[0x02] = (value & 0x00000003);
        update_host_irq(p);
        break;

    case 0x03: /* irq flags */
        p->hostRegs[0x03] &= ~(value & 0x00000003);
        update_host_irq(p);
        break;

    case 0x04: /* vlan mask */
        break;

    case 0x05: /* filter select */
        break;

    case 0x06: /* filter control */
        break;

    case 0x07: /* filter load */
        break;

    case 0x08: /* bad packet */
        break;

    case 0x0F: /* revision */
        break;

    default:
        printf("labx-ethernet: Write of unknown register %"HWADDR_PRIX" = %08X\n",
               addr, value);
        break;
    }
}

static const MemoryRegionOps ethernet_regs_ops = {
    .read = ethernet_regs_read,
    .write = ethernet_regs_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
    .valid = {
        .min_access_size = 4,
        .max_access_size = 4
    }
};


/*
 * MAC registers
 */
static uint64_t mac_regs_read(void *opaque, hwaddr addr,
                              unsigned int size)
{
    /*LabXEthernet *p = opaque; */

    uint32_t retval = 0;

    switch ((addr>>2) & 0x0F) {
    case 0x01: /* host rx config */
        break;

    case 0x02: /* host tx config */
        break;

    case 0x04: /* host speed config */
        break;

    case 0x05: /* host mdio config */
        break;

    default:
        printf("labx-ethernet: Read of unknown mac register %"HWADDR_PRIX"\n", addr);
        break;
    }

    return retval;
}

static void mac_regs_write(void *opaque, hwaddr addr,
                           uint64_t val64, unsigned int size)
{
    /*LabXEthernet *p = opaque; */
    uint32_t value = val64;

    switch ((addr>>2) & 0x0F) {
    case 0x01: /* host rx config */
        break;

    case 0x02: /* host tx config */
        break;

    case 0x04: /* host speed config */
        break;

    case 0x05: /* host mdio config */
        break;

    default:
        printf("labx-ethernet: Write of unknown mac register %"HWADDR_PRIX" = %08X\n",
               addr, value);
        break;
    }
}

static const MemoryRegionOps mac_regs_ops = {
    .read = mac_regs_read,
    .write = mac_regs_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
    .valid = {
        .min_access_size = 4,
        .max_access_size = 4
    }
};


/*
 * FIFO registers
 */

#define FIFO_INT_STATUS_ADDRESS   0x0
#define FIFO_INT_ENABLE_ADDRESS   0x1
#  define FIFO_INT_RPURE 0x80000000
#  define FIFO_INT_RPORE 0x40000000
#  define FIFO_INT_RPUE  0x20000000
#  define FIFO_INT_TPOE  0x10000000
#  define FIFO_INT_TC    0x08000000
#  define FIFO_INT_RC    0x04000000
#  define FIFO_INT_MASK  0xFC000000
#define FIFO_TX_RESET_ADDRESS     0x2
#  define FIFO_RESET_MAGIC 0xA5
#define FIFO_TX_VACANCY_ADDRESS   0x3
#define FIFO_TX_DATA_ADDRESS      0x4
#define FIFO_TX_LENGTH_ADDRESS    0x5
#define FIFO_RX_RESET_ADDRESS     0x6
#define FIFO_RX_OCCUPANCY_ADDRESS 0x7
#define FIFO_RX_DATA_ADDRESS      0x8
#define FIFO_RX_LENGTH_ADDRESS    0x9

static void update_fifo_irq(LabXEthernet *p)
{
    if ((p->fifoRegs[FIFO_INT_STATUS_ADDRESS] &
         p->fifoRegs[FIFO_INT_ENABLE_ADDRESS]) != 0) {
        qemu_irq_raise(p->fifoIrq);
    } else {
        qemu_irq_lower(p->fifoIrq);
    }
}

static void send_packet(LabXEthernet *p)
{
    while (p->txLengthPopIndex != p->txLengthPushIndex) {
        int i;
        uint32_t packetBuf[512];

        int length = p->txLengthBuffer[p->txLengthPopIndex];
        p->txLengthPopIndex = (p->txLengthPopIndex + 1) % LENGTH_FIFO_WORDS;

        for (i = 0; i < ((length+3)/4); i++) {
            packetBuf[i] = be32_to_cpu(p->txBuffer[p->txPopIndex]);
            p->txPopIndex = (p->txPopIndex + 1) % (FIFO_RAM_BYTES/4);
        }

        qemu_send_packet(qemu_get_queue(p->nic), (void *)packetBuf, length);
    }

    p->fifoRegs[FIFO_INT_STATUS_ADDRESS] |= FIFO_INT_TC;
    update_fifo_irq(p);
}

static uint64_t fifo_regs_read(void *opaque, hwaddr addr,
                               unsigned int size)
{
    LabXEthernet *p = opaque;

    uint32_t retval = 0;

    switch ((addr>>2) & 0x0F) {
    case FIFO_INT_STATUS_ADDRESS:
    case FIFO_INT_ENABLE_ADDRESS:
    case FIFO_TX_RESET_ADDRESS:
        retval = p->fifoRegs[(addr>>2) & 0x0F];
        break;

    case FIFO_TX_VACANCY_ADDRESS:
        retval = (p->txPopIndex - p->txPushIndex) - 1;
        if ((int32_t)retval < 0) {
            retval += (FIFO_RAM_BYTES/4);
        }

        if (((p->txLengthPushIndex + 1) % LENGTH_FIFO_WORDS) ==
            p->txLengthPopIndex) {
            /* Full length fifo */
            retval = 0;
        }
        break;

    case FIFO_TX_DATA_ADDRESS:
    case FIFO_TX_LENGTH_ADDRESS:
    case FIFO_RX_RESET_ADDRESS:
        retval = p->fifoRegs[(addr>>2) & 0x0F];
        break;

    case FIFO_RX_OCCUPANCY_ADDRESS:
        retval = p->rxPushIndex - p->rxPopIndex;
        if ((int32_t)retval < 0) {
            retval += (FIFO_RAM_BYTES/4);
        }
        break;

    case FIFO_RX_DATA_ADDRESS:
        retval = p->rxBuffer[p->rxPopIndex];
        if (p->rxPopIndex != p->rxPushIndex) {
            p->rxPopIndex = (p->rxPopIndex+1) % (FIFO_RAM_BYTES/4);
        } else {
            p->fifoRegs[FIFO_INT_STATUS_ADDRESS] |= FIFO_INT_RPURE;
            update_fifo_irq(p);
        }
        break;

    case FIFO_RX_LENGTH_ADDRESS:
        retval = p->rxLengthBuffer[p->rxLengthPopIndex];
        if (p->rxLengthPopIndex != p->rxLengthPushIndex) {
            p->rxLengthPopIndex = (p->rxLengthPopIndex+1) % LENGTH_FIFO_WORDS;
        } else {
            p->fifoRegs[FIFO_INT_STATUS_ADDRESS] |= FIFO_INT_RPURE;
            update_fifo_irq(p);
        }
        break;

    default:
        printf("labx-ethernet: Read of unknown fifo register %"HWADDR_PRIX"\n", addr);
        break;
    }

    /* printf("FIFO REG READ %08X (%d) = %08X\n",
               addr, (addr>>2) & 0x0F, retval); */

    return retval;
}

static void fifo_regs_write(void *opaque, hwaddr addr,
                            uint64_t val64, unsigned int size)
{
    LabXEthernet *p = opaque;
    uint32_t value = val64;

    /* printf("FIFO REG WRITE %08X (%d) = %08X\n",
              addr, (addr>>2) & 0x0F, value); */

    switch ((addr>>2) & 0x0F) {
    case FIFO_INT_STATUS_ADDRESS:
        p->fifoRegs[FIFO_INT_STATUS_ADDRESS] &= ~(value & FIFO_INT_MASK);
        update_fifo_irq(p);
        break;

    case FIFO_INT_ENABLE_ADDRESS:
        p->fifoRegs[FIFO_INT_ENABLE_ADDRESS] = (value & FIFO_INT_MASK);
        update_fifo_irq(p);
        break;

    case FIFO_TX_RESET_ADDRESS:
        if (value == FIFO_RESET_MAGIC) {
            p->txPushIndex = 0;
            p->txPopIndex = 0;
            p->txLengthPushIndex = 0;
            p->txLengthPopIndex = 0;
        }
        break;

    case FIFO_TX_VACANCY_ADDRESS:
        break;

    case FIFO_TX_DATA_ADDRESS:
        if ((((p->txLengthPushIndex + 1) % LENGTH_FIFO_WORDS) ==
             p->txLengthPopIndex) ||
            (((p->txPushIndex + 1) % (FIFO_RAM_BYTES/4)) == p->txPopIndex)) {
            /* Full length fifo or data fifo */
            p->fifoRegs[FIFO_INT_STATUS_ADDRESS] |= FIFO_INT_TPOE;
            update_fifo_irq(p);
        } else {
            /* Push back the data */
            p->txBuffer[p->txPushIndex] = value;
            p->txPushIndex = (p->txPushIndex + 1) % (FIFO_RAM_BYTES/4);
        }
        break;

    case FIFO_TX_LENGTH_ADDRESS:
        if (((p->txLengthPushIndex + 1) % LENGTH_FIFO_WORDS) ==
            p->txLengthPopIndex) {
            /* Full length fifo */
            p->fifoRegs[FIFO_INT_STATUS_ADDRESS] |= FIFO_INT_TPOE;
            update_fifo_irq(p);
        } else {
            /* Push back the length */
            p->txLengthBuffer[p->txLengthPushIndex] = value;
            p->txLengthPushIndex = (p->txLengthPushIndex + 1) %
                                   LENGTH_FIFO_WORDS;
            send_packet(p);
        }
        break;

    case FIFO_RX_RESET_ADDRESS:
        if (value == FIFO_RESET_MAGIC) {
            p->rxPushIndex = 0;
            p->rxPopIndex = 0;
            p->rxLengthPushIndex = 0;
            p->rxLengthPopIndex = 0;
        }
        break;

    case FIFO_RX_OCCUPANCY_ADDRESS:
        break;

    case FIFO_RX_DATA_ADDRESS:
        break;

    case FIFO_RX_LENGTH_ADDRESS:
        break;

    default:
        printf("labx-ethernet: Write of unknown fifo register %"HWADDR_PRIX" = %08X\n",
               addr, value);
        break;
    }
}

static const MemoryRegionOps fifo_regs_ops = {
    .read = fifo_regs_read,
    .write = fifo_regs_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
    .valid = {
        .min_access_size = 4,
        .max_access_size = 4
    }
};


static int eth_can_rx(NetClientState *nc)
{
    /*LabXEthernet *s = DO_UPCAST(NICState, nc, nc)->opaque; */

    return 1;
}

static ssize_t eth_rx(NetClientState *nc, const uint8_t *buf, size_t size)
{
    LabXEthernet *p = qemu_get_nic_opaque(nc);
    int i;
    const uint32_t *wbuf = (const uint32_t *)buf;
    int rxPushIndexStart = p->rxPushIndex;

    for (i = 0; i < ((size+3)/4); i++) {
        p->rxBuffer[p->rxPushIndex] = cpu_to_be32(wbuf[i]);
        p->rxPushIndex = (p->rxPushIndex + 1) % (FIFO_RAM_BYTES/4);
        if (p->rxPushIndex == p->rxPopIndex) {
            /* Packet didn't fit */
            p->rxPushIndex = rxPushIndexStart;
            return -1;
        }
    }

    if ((p->rxLengthPushIndex + 1) % LENGTH_FIFO_WORDS == p->rxLengthPopIndex) {
        /* Length didn't fit */
        p->rxPushIndex = rxPushIndexStart;
        return -1;
    }

    p->rxLengthBuffer[p->rxLengthPushIndex] = size;
    p->rxLengthPushIndex = (p->rxLengthPushIndex + 1) % LENGTH_FIFO_WORDS;

    p->fifoRegs[FIFO_INT_STATUS_ADDRESS] |= FIFO_INT_RC;
    update_fifo_irq(p);

    return size;
}

static void eth_cleanup(NetClientState *nc)
{
    LabXEthernet *s = qemu_get_nic_opaque(nc);

    s->nic = NULL;
}

static NetClientInfo net_labx_ethernet_info = {
    .type = NET_CLIENT_DRIVER_NIC,
    .size = sizeof(NICState),
    .can_receive = eth_can_rx,
    .receive = eth_rx,
    .cleanup = eth_cleanup,
};

static int labx_ethernet_init(SysBusDevice *dev)
{
    LabXEthernet *p = LABX_ETHERNET(dev);

    /* Initialize defaults */
    p->txBuffer = g_malloc0(FIFO_RAM_BYTES);
    p->txLengthBuffer = g_malloc0(LENGTH_FIFO_WORDS*4);
    p->rxBuffer = g_malloc0(FIFO_RAM_BYTES);
    p->rxLengthBuffer = g_malloc0(LENGTH_FIFO_WORDS*4);

    p->txPushIndex = 0;
    p->txPopIndex = 0;
    p->txLengthPushIndex = 0;
    p->txLengthPopIndex = 0;
    p->rxPushIndex = 0;
    p->rxPopIndex = 0;
    p->rxLengthPushIndex = 0;
    p->rxLengthPopIndex = 0;

    /* Set up memory regions */
    memory_region_init_io(&p->mmio_ethernet, OBJECT(p), &ethernet_regs_ops, p,
                          "labx.ethernet-regs",      0x10 * 4);
    memory_region_init_io(&p->mmio_mac,      OBJECT(p), &mac_regs_ops,      p,
                          "labx.ethernet-mac-regs",  0x10 * 4);
    memory_region_init_io(&p->mmio_fifo,     OBJECT(p), &fifo_regs_ops,     p,
                          "labx.ethernet-fifo-regs", 0x10 * 4);

    sysbus_init_mmio(dev, &p->mmio_ethernet);
    sysbus_init_mmio(dev, &p->mmio_mac);
    sysbus_init_mmio(dev, &p->mmio_fifo);

    /* Offset 0 is automatically mapped. Map the other regions */
    sysbus_mmio_map(dev, 1, p->baseAddress + (1 << (10+2)));
    sysbus_mmio_map(dev, 2, p->baseAddress + (2 << (10+2)));

    /* Initialize the irqs */
    sysbus_init_irq(dev, &p->hostIrq);
    sysbus_init_irq(dev, &p->fifoIrq);
    sysbus_init_irq(dev, &p->phyIrq);

    /* Set up the NIC */
    qemu_macaddr_default_if_unset(&p->conf.macaddr);
    p->nic = qemu_new_nic(&net_labx_ethernet_info, &p->conf,
                          object_get_typename(OBJECT(p)), DEVICE(p)->id, p);
    qemu_format_nic_info_str(qemu_get_queue(p->nic), p->conf.macaddr.a);
    return 0;
}

static Property labx_ethernet_properties[] = {
    DEFINE_PROP_UINT32("reg", LabXEthernet, baseAddress, 0),
    DEFINE_NIC_PROPERTIES(LabXEthernet, conf),
    DEFINE_PROP_END_OF_LIST(),
};

static void labx_ethernet_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    SysBusDeviceClass *k = SYS_BUS_DEVICE_CLASS(klass);

    k->init = labx_ethernet_init;
    dc->props = labx_ethernet_properties;
}

static const TypeInfo labx_ethernet_info = {
    .name          = TYPE_LABX_ETHERNET,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(LabXEthernet),
    .class_init    = labx_ethernet_class_init,
};

static const TypeInfo labx_ethernet_info2 = {
    .name          = "xlnx.labx-ethernet",
    .parent        = TYPE_LABX_ETHERNET,
    .instance_size = sizeof(LabXEthernet),
};

static const TypeInfo labx_ethernet_info3 = {
    .name          = "biamp,ethernet-1.04a",
    .parent        = TYPE_LABX_ETHERNET,
    .instance_size = sizeof(LabXEthernet),
};

static void labx_ethernet_register(void)
{
    type_register_static(&labx_ethernet_info);
    type_register_static(&labx_ethernet_info2);
    type_register_static(&labx_ethernet_info3);
}

type_init(labx_ethernet_register)
