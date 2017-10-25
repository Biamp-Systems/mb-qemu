
/*
 * QEMU model of the LabX DMA Engine.
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

#define TYPE_LABX_DMA "labx.dma"
#define LABX_DMA(obj) \
    OBJECT_CHECK(LabXDMA, (obj), TYPE_LABX_DMA)

typedef struct LabXDMA {
    SysBusDevice busdev;

    MemoryRegion  mmio_dma;
    MemoryRegion  mmio_microcode;

    /* IRQ */
    qemu_irq irq;

    /* Device Configuration */
    uint32_t baseAddress;
    uint32_t paramWords;
    uint32_t microcodeWords;
    uint32_t numIndexRegs;
    uint32_t numChannels;
    uint32_t numAlus;
    uint32_t hasStatusFifo;

    /* Values set by drivers */

    /* Microcode buffer */
    uint32_t *microcodeRam;
} LabXDMA;

/*
 * DMA registers
 */
static uint64_t dma_regs_read(void *opaque, hwaddr addr,
                              unsigned int size)
{
    LabXDMA *p = opaque;

    uint32_t retval = 0;

    if ((addr>>2) & 0x80) {
        /* vector */
    } else {
        switch ((addr>>2) & 0x7F) {
        case 0x00: /* control */
            break;

        case 0x01: /* channel enable */
            break;

        case 0x02: /* channel start */
            break;

        case 0x03: /* channel irq enable */
            break;

        case 0x04: /* channel irq */
            break;

        case 0x05: /* sync */
            break;

        case 0x7E: /* capabilities */
            retval = ((p->hasStatusFifo              & 0x01) << 16) |
                     ((p->numIndexRegs               & 0x0F) << 12) |
                     ((p->numChannels                & 0x03) << 10) |
                     ((p->numAlus                    & 0x03) << 8) |
                     ((min_bits(p->paramWords-1)     & 0x0F) << 4) |
                     ((min_bits(p->microcodeWords-1) & 0x0F));
            break;

        case 0x7F: /* revision */
            retval = 0x00000011;
            break;

        default:
            printf("labx-dma: Read of unknown register %"HWADDR_PRIX"\n", addr);
            break;
        }
    }

    return retval;
}

static void dma_regs_write(void *opaque, hwaddr addr,
                           uint64_t val64, unsigned int size)
{
    /*LabXDMA *p = opaque; */
    uint32_t value = val64;

    if ((addr>>2) & 0x80) {
        /* vector */
    } else {
        switch ((addr>>2) & 0x7F) {
        case 0x00: /* control */
            break;

        case 0x01: /* channel enable */
            break;

        case 0x02: /* channel start */
            break;

        case 0x03: /* channel irq enable */
            break;

        case 0x04: /* channel irq */
            break;

        case 0x05: /* sync */
            break;

        case 0x7E: /* capabilities */
            break;

        case 0x7F: /* revision */
            break;

        default:
            printf("labx-dma: Write of unknown register "
                   "%"HWADDR_PRIX" = %08X\n", addr, value);
            break;
        }
    }
}

static const MemoryRegionOps dma_regs_ops = {
    .read = dma_regs_read,
    .write = dma_regs_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
    .valid = {
        .min_access_size = 4,
        .max_access_size = 4
    }
};


/*
 * Microcode RAM
 */
static uint64_t microcode_ram_read(void *opaque, hwaddr addr,
                                   unsigned int size)
{
    LabXDMA *p = opaque;

    return p->microcodeRam[RAM_INDEX(addr, p->microcodeWords)];
}

static void microcode_ram_write(void *opaque, hwaddr addr,
                                uint64_t val64, unsigned int size)
{
    LabXDMA *p = opaque;
    uint32_t value = val64;

    p->microcodeRam[RAM_INDEX(addr, p->microcodeWords)] = value;
}

static const MemoryRegionOps microcode_ram_ops = {
    .read = microcode_ram_read,
    .write = microcode_ram_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
    .valid = {
        .min_access_size = 4,
        .max_access_size = 4
    }
};


static int labx_dma_init(SysBusDevice *dev)
{
    LabXDMA *p = LABX_DMA(dev);

    /* Initialize defaults */
    p->microcodeRam = g_malloc0(p->microcodeWords*4);

    /* Set up the IRQ */
    sysbus_init_irq(dev, &p->irq);

    /* Set up memory regions */
    memory_region_init_io(&p->mmio_dma,       OBJECT(p), &dma_regs_ops,      p,
                          "labx,dma-regs",      0x100 * 4);
    memory_region_init_io(&p->mmio_microcode, OBJECT(p), &microcode_ram_ops, p,
                          "labx,dma-microcode", 4 * p->microcodeWords);

    sysbus_init_mmio(dev, &p->mmio_dma);
    sysbus_init_mmio(dev, &p->mmio_microcode);

    /* Offset 0 is automatically mapped. Map the other regions */
    sysbus_mmio_map(dev, 1, p->baseAddress +
                            (1 << (min_bits(p->microcodeWords-1)+2)));

    return 0;
}

static Property labx_dma_properties[] = {
    DEFINE_PROP_UINT32("reg",             LabXDMA, baseAddress,    0),
    DEFINE_PROP_UINT32("param-words",     LabXDMA, paramWords,     1024),
    DEFINE_PROP_UINT32("microcode-words", LabXDMA, microcodeWords, 1024),
    DEFINE_PROP_UINT32("num-index-regs",  LabXDMA, numIndexRegs,   4),
    DEFINE_PROP_UINT32("dma-num-channels",LabXDMA, numChannels,    1),
    DEFINE_PROP_UINT32("num-alus",        LabXDMA, numAlus,        1),
    DEFINE_PROP_UINT32("status-fifo",     LabXDMA, hasStatusFifo,  1),
    DEFINE_PROP_END_OF_LIST(),
};

static void labx_dma_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    SysBusDeviceClass *k = SYS_BUS_DEVICE_CLASS(klass);

    k->init = labx_dma_init;
    dc->props = labx_dma_properties;
}

static const TypeInfo labx_dma_info = {
    .name          = TYPE_LABX_DMA,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(LabXDMA),
    .class_init    = labx_dma_class_init,
};

static const TypeInfo labx_dma_info2 = {
    .name          = "xlnx.labx-dma",
    .parent        = TYPE_LABX_DMA,
    .instance_size = sizeof(LabXDMA),
};

static void labrinth_tdm_output_class_init(ObjectClass *klass, void *data)
{
    // TODO: add on the TDM/LA registers
}

static const TypeInfo labrinth_tdm_output_info = {
    .name          = "xlnx.labrinth-tdm-output",
    .parent        = TYPE_LABX_DMA,
    .instance_size = sizeof(LabXDMA),
    .class_init    = labrinth_tdm_output_class_init,
};

static void labx_dma_register(void)
{
    type_register_static(&labx_dma_info);
    type_register_static(&labx_dma_info2);
    type_register_static(&labrinth_tdm_output_info);
}

type_init(labx_dma_register)
