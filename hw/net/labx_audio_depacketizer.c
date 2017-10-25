
/*
 * QEMU model of the LabX audio depacketizer.
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
#include "hw/labx_devices.h"

#define min_bits(i) (32 - clz32((i)))
#define RAM_INDEX(addr, size) (((addr)>>2)&((1<<min_bits((size)-1))-1))

#define TYPE_DEPACKETIZER "labx.audio-depacketizer"
#define DEPACKETIZER(obj) \
    OBJECT_CHECK(Depacketizer, (obj), TYPE_DEPACKETIZER)

typedef struct ClockDomainInfo {
    uint32_t tsInterval;
} ClockDomainInfo;

typedef struct Depacketizer {
    SysBusDevice busdev;

    MemoryRegion  mmio_depacketizer;
    MemoryRegion  mmio_clock_domain;
    MemoryRegion  mmio_microcode;

    /* Device Configuration */
    uint32_t baseAddress;
    uint32_t clockDomains;
    uint32_t cacheDataWords;
    uint32_t paramWords;
    uint32_t microcodeWords;
    uint32_t maxStreamSlots;
    uint32_t maxStreams;
    char    *interfaceType;
    uint32_t matchArch;

    /* IRQ */
    qemu_irq irq;

    /* Values set by drivers */

    /* Microcode buffer */
    uint32_t *microcodeRam;

    /* Clock domain information */
    ClockDomainInfo *clockDomainInfo;

    /* Attached DMA (if interfaceType != CACHE_RAM) */
    DeviceState *dma;
} Depacketizer;

/*
 * Depacketizer registers
 */
static uint64_t depacketizer_regs_read(void *opaque, hwaddr addr,
                                       unsigned int size)
{
    Depacketizer *p = opaque;

    uint32_t retval = 0;

    switch ((addr>>2) & 0xFF) {
    case 0x00: /* control */
        break;

    case 0x01: /* vector bar */
        break;

    case 0x02: /* id select 0 */
        break;

    case 0x03: /* id select 1 */
        break;

    case 0x04: /* id select 2 */
        break;

    case 0x05: /* id select 3 */
        break;

    case 0x06: /* id config data */
        break;

    case 0x08: /* irq mask */
        break;

    case 0x09: /* irq flags */
        break;

    case 0x0A: /* sync */
        break;

    case 0x0B: /* relocate */
        break;

    case 0x0C: /* stream status 0 */
        break;

    case 0x0D: /* stream status 1 */
        break;

    case 0x0E: /* stream status 2 */
        break;

    case 0x0F: /* stream status 3 */
        break;

    case 0xFD: /* capabilities a */
        retval = (p->maxStreamSlots & 0x7F);
        break;

    case 0xFE: /* capabilities b */
        retval = ((p->matchArch & 0xFF) << 24) |
                 ((p->maxStreams & 0xFF) << 16) |
                 ((p->clockDomains & 0xFF) << 8) |
                 ((min_bits(p->paramWords-1) & 0x0F) << 4) |
                 ((min_bits(p->microcodeWords-1) & 0x0F));
        break;

    case 0xFF: /* revision */
        retval = 0x00000014;
        break;

    default:
        printf("labx-audio-depacketizer: Read of unknown register %"HWADDR_PRIX"\n",
               addr);
        break;
    }

    return retval;
}

static void depacketizer_regs_write(void *opaque, hwaddr addr,
                                    uint64_t val64, unsigned int size)
{
    /*Depacketizer *p = opaque; */
    uint32_t value = val64;

    switch ((addr>>2) & 0xFF) {
    case 0x00: /* control */
        break;

    case 0x01: /* vector bar */
        break;

    case 0x02: /* id select 0 */
        break;

    case 0x03: /* id select 1 */
        break;

    case 0x04: /* id select 2 */
        break;

    case 0x05: /* id select 3 */
        break;

    case 0x06: /* id config data */
        break;

    case 0x08: /* irq mask */
        break;

    case 0x09: /* irq flags */
        break;

    case 0x0A: /* sync */
        break;

    case 0x0B: /* relocate */
        break;

    case 0x0C: /* stream status 0 */
        break;

    case 0x0D: /* stream status 1 */
        break;

    case 0x0E: /* stream status 2 */
        break;

    case 0x0F: /* stream status 3 */
        break;

    case 0xFD: /* capabilities a */
        break;

    case 0xFE: /* capabilities b */
        break;

    case 0xFF: /* revision */
        break;

    default:
        printf("labx-audio-depacketizer: Write of unknown register "
               "%"HWADDR_PRIX" = %08X\n", addr, value);
        break;
    }
}

static const MemoryRegionOps depacketizer_regs_ops = {
    .read = depacketizer_regs_read,
    .write = depacketizer_regs_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
    .valid = {
        .min_access_size = 4,
        .max_access_size = 4
    }
};


/*
 * Clock domain registers
 */
static uint64_t clock_domain_regs_read(void *opaque, hwaddr addr,
                                       unsigned int size)
{
    Depacketizer *p = opaque;

    uint32_t retval = 0;
    int domain = (addr>>6) & ((1<<min_bits(p->clockDomains-1))-1);

    switch ((addr>>2)&0x10) {
    case 0x00: /* recovery index */
        break;

    case 0x01: /* ts interval */
        retval = p->clockDomainInfo[domain].tsInterval;
        break;

    case 0x08: /* DAC offset */
        break;

    case 0x09: /* DAC P coeff */
        break;

    case 0x0A: /* lock count */
        break;

    default:
        break;
    }

    return retval;
}

static void clock_domain_regs_write(void *opaque, hwaddr addr,
                                    uint64_t val64, unsigned int size)
{
    Depacketizer *p = opaque;
    uint32_t value = val64;
    int domain = (addr>>6) & ((1<<min_bits(p->clockDomains-1))-1);

    switch ((addr>>2)&0x10) {
    case 0x00: /* recovery index */
        break;

    case 0x01: /* ts interval */
        p->clockDomainInfo[domain].tsInterval = value;
        break;

    case 0x08: /* DAC offset */
        break;

    case 0x09: /* DAC P coeff */
        break;

    case 0x0A: /* lock count */
        break;

    default:
        break;
    }
}

static const MemoryRegionOps clock_domain_regs_ops = {
    .read = clock_domain_regs_read,
    .write = clock_domain_regs_write,
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
    Depacketizer *p = opaque;

    return p->microcodeRam[RAM_INDEX(addr, p->microcodeWords)];
}

static void microcode_ram_write(void *opaque, hwaddr addr,
                               uint64_t val64, unsigned int size)
{
    Depacketizer *p = opaque;
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


static int labx_audio_depacketizer_init(SysBusDevice *dev)
{
    Depacketizer *p = DEPACKETIZER(dev);

    /* Initialize defaults */
    p->microcodeRam = g_malloc0(p->microcodeWords*4);
    p->clockDomainInfo = g_malloc0(sizeof(ClockDomainInfo) *
                                   p->clockDomains);

    /* Set up the IRQ */
    sysbus_init_irq(dev, &p->irq);

    /* Set up memory regions */
    memory_region_init_io(&p->mmio_depacketizer, OBJECT(p), &depacketizer_regs_ops, p,
                          "labx.audio-depacketizer-regs",
                          0x100 * 4);
    memory_region_init_io(&p->mmio_clock_domain, OBJECT(p), &clock_domain_regs_ops, p,
                          "labx.audio-depacketizer-cd-regs",
                          0x10 * 4 * p->clockDomains);
    memory_region_init_io(&p->mmio_microcode,    OBJECT(p), &microcode_ram_ops,     p,
                          "labx.audio-depacketizer-microcode",
                          4 * p->microcodeWords);

    sysbus_init_mmio(dev, &p->mmio_depacketizer);
    sysbus_init_mmio(dev, &p->mmio_clock_domain);
    sysbus_init_mmio(dev, &p->mmio_microcode);

    /* Offset 0 is automatically mapped. Map the other regions */
    sysbus_mmio_map(dev, 1, p->baseAddress +
                            (1 << (min_bits(p->microcodeWords-1)+2)));
    sysbus_mmio_map(dev, 2, p->baseAddress +
                            (2 << (min_bits(p->microcodeWords-1)+2)));

    if (!p->interfaceType || strcmp(p->interfaceType, "CACHE_RAM")) {
        p->dma = labx_dma_create(p->baseAddress +
                                 (4 << (min_bits(p->microcodeWords-1)+2)),
                                 1024);
    }

    return 0;
}

static Property labx_audio_depacketizer_properties[] = {
    DEFINE_PROP_UINT32("reg",               Depacketizer, baseAddress,    0),
    DEFINE_PROP_UINT32("num-clock-domains", Depacketizer, clockDomains,   1),
    DEFINE_PROP_UINT32("cache-data-words",  Depacketizer, cacheDataWords, 1024),
    DEFINE_PROP_UINT32("param-words",       Depacketizer, paramWords,     1024),
    DEFINE_PROP_UINT32("microcode-words",   Depacketizer, microcodeWords, 1024),
    DEFINE_PROP_UINT32("max-stream-slots",  Depacketizer, maxStreamSlots, 32),
    DEFINE_PROP_UINT32("max-streams",       Depacketizer, maxStreams,     128),
    DEFINE_PROP_STRING("interface-type",    Depacketizer, interfaceType      ),
    DEFINE_PROP_UINT32("match-arch",        Depacketizer, matchArch,      255),
    DEFINE_PROP_END_OF_LIST(),
};

static void labx_audio_depacketizer_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    SysBusDeviceClass *k = SYS_BUS_DEVICE_CLASS(klass);

    k->init = labx_audio_depacketizer_init;
    dc->props = labx_audio_depacketizer_properties;
}

static const TypeInfo labx_audio_depacketizer_info = {
    .name          = TYPE_DEPACKETIZER,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(Depacketizer),
    .class_init    = labx_audio_depacketizer_class_init,
};

static const TypeInfo labx_audio_depacketizer_info2 = {
    .name          = "xlnx.labx-audio-depacketizer",
    .parent        = TYPE_DEPACKETIZER,
    .instance_size = sizeof(Depacketizer),
};

static void labx_audio_depacketizer_register(void)
{
    type_register_static(&labx_audio_depacketizer_info);
    type_register_static(&labx_audio_depacketizer_info2);
}

type_init(labx_audio_depacketizer_register)
