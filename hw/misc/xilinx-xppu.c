/*
 * QEMU model of the XPPU
 *
 * Copyright (c) 2014 - 2016 Xilinx Inc.
 *
 * Autogenerated by xregqemu.py 2016-02-14.
 * Written by Edgar E. Iglesias <edgar.iglesias@xilinx.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "qemu/osdep.h"
#include "hw/sysbus.h"
#include "hw/register-dep.h"
#include "qemu/bitops.h"
#include "sysemu/dma.h"
#include "qapi/error.h"
#include "qemu/log.h"

#include "hw/fdt_generic_util.h"

#ifndef XILINX_XPPU_ERR_DEBUG
#define XILINX_XPPU_ERR_DEBUG 0
#endif

#define TYPE_XILINX_XPPU "xlnx.xppu"

#define XILINX_XPPU(obj) \
     OBJECT_CHECK(XPPU, (obj), TYPE_XILINX_XPPU)

DEP_REG32(CTRL, 0x0)
    DEP_FIELD(CTRL, APER_PARITY_EN, 1, 2)
    DEP_FIELD(CTRL, MID_PARITY_EN, 1, 1)
    DEP_FIELD(CTRL, ENABLE, 1, 0)
DEP_REG32(ERR_STATUS1, 0x4)
    DEP_FIELD(ERR_STATUS1, AXI_ADDR, 20, 0)
DEP_REG32(ERR_STATUS2, 0x8)
    DEP_FIELD(ERR_STATUS2, AXI_ID, 10, 0)
DEP_REG32(POISON, 0xc)
    DEP_FIELD(POISON, BASE, 20, 0)
DEP_REG32(ISR, 0x10)
    DEP_FIELD(ISR, APER_PARITY, 1, 7)
    DEP_FIELD(ISR, APER_TZ, 1, 6)
    DEP_FIELD(ISR, APER_PERM, 1, 5)
    DEP_FIELD(ISR, MID_PARITY, 1, 3)
    DEP_FIELD(ISR, MID_RO, 1, 2)
    DEP_FIELD(ISR, MID_MISS, 1, 1)
    DEP_FIELD(ISR, INV_APB, 1, 0)
DEP_REG32(IMR, 0x14)
    DEP_FIELD(IMR, APER_PARITY, 1, 7)
    DEP_FIELD(IMR, APER_TZ, 1, 6)
    DEP_FIELD(IMR, APER_PERM, 1, 5)
    DEP_FIELD(IMR, MID_PARITY, 1, 3)
    DEP_FIELD(IMR, MID_RO, 1, 2)
    DEP_FIELD(IMR, MID_MISS, 1, 1)
    DEP_FIELD(IMR, INV_APB, 1, 0)
DEP_REG32(IEN, 0x18)
    DEP_FIELD(IEN, APER_PARITY, 1, 7)
    DEP_FIELD(IEN, APER_TZ, 1, 6)
    DEP_FIELD(IEN, APER_PERM, 1, 5)
    DEP_FIELD(IEN, MID_PARITY, 1, 3)
    DEP_FIELD(IEN, MID_RO, 1, 2)
    DEP_FIELD(IEN, MID_MISS, 1, 1)
    DEP_FIELD(IEN, INV_APB, 1, 0)
DEP_REG32(IDS, 0x1c)
    DEP_FIELD(IDS, APER_PARITY, 1, 7)
    DEP_FIELD(IDS, APER_TZ, 1, 6)
    DEP_FIELD(IDS, APER_PERM, 1, 5)
    DEP_FIELD(IDS, MID_PARITY, 1, 3)
    DEP_FIELD(IDS, MID_RO, 1, 2)
    DEP_FIELD(IDS, MID_MISS, 1, 1)
    DEP_FIELD(IDS, INV_APB, 1, 0)
DEP_REG32(M_MASTER_IDS, 0x3c)
DEP_REG32(M_APERTURE_32B, 0x40)
DEP_REG32(M_APERTURE_64KB, 0x44)
DEP_REG32(M_APERTURE_1MB, 0x48)
DEP_REG32(M_APERTURE_512MB, 0x4c)
DEP_REG32(BASE_32B, 0x50)
DEP_REG32(BASE_64KB, 0x54)
DEP_REG32(BASE_1MB, 0x58)
DEP_REG32(BASE_512MB, 0x5c)
DEP_REG32(ECO, 0xfc)
DEP_REG32(MASTER_ID00, 0x100)
    DEP_FIELD(MASTER_ID00, MIDP, 1, 31)
    DEP_FIELD(MASTER_ID00, MIDR, 1, 30)
    DEP_FIELD(MASTER_ID00, MIDM, 10, 16)
    DEP_FIELD(MASTER_ID00, MID, 10, 0)
DEP_REG32(MASTER_ID01, 0x104)
    DEP_FIELD(MASTER_ID01, MIDP, 1, 31)
    DEP_FIELD(MASTER_ID01, MIDR, 1, 30)
    DEP_FIELD(MASTER_ID01, MIDM, 10, 16)
    DEP_FIELD(MASTER_ID01, MID, 10, 0)
DEP_REG32(MASTER_ID02, 0x108)
    DEP_FIELD(MASTER_ID02, MIDP, 1, 31)
    DEP_FIELD(MASTER_ID02, MIDR, 1, 30)
    DEP_FIELD(MASTER_ID02, MIDM, 10, 16)
    DEP_FIELD(MASTER_ID02, MID, 10, 0)
DEP_REG32(MASTER_ID03, 0x10c)
    DEP_FIELD(MASTER_ID03, MIDP, 1, 31)
    DEP_FIELD(MASTER_ID03, MIDR, 1, 30)
    DEP_FIELD(MASTER_ID03, MIDM, 10, 16)
    DEP_FIELD(MASTER_ID03, MID, 10, 0)
DEP_REG32(MASTER_ID04, 0x110)
    DEP_FIELD(MASTER_ID04, MIDP, 1, 31)
    DEP_FIELD(MASTER_ID04, MIDR, 1, 30)
    DEP_FIELD(MASTER_ID04, MIDM, 10, 16)
    DEP_FIELD(MASTER_ID04, MID, 10, 0)
DEP_REG32(MASTER_ID05, 0x114)
    DEP_FIELD(MASTER_ID05, MIDP, 1, 31)
    DEP_FIELD(MASTER_ID05, MIDR, 1, 30)
    DEP_FIELD(MASTER_ID05, MIDM, 10, 16)
    DEP_FIELD(MASTER_ID05, MID, 10, 0)
DEP_REG32(MASTER_ID06, 0x118)
    DEP_FIELD(MASTER_ID06, MIDP, 1, 31)
    DEP_FIELD(MASTER_ID06, MIDR, 1, 30)
    DEP_FIELD(MASTER_ID06, MIDM, 10, 16)
    DEP_FIELD(MASTER_ID06, MID, 10, 0)
DEP_REG32(MASTER_ID07, 0x11c)
    DEP_FIELD(MASTER_ID07, MIDP, 1, 31)
    DEP_FIELD(MASTER_ID07, MIDR, 1, 30)
    DEP_FIELD(MASTER_ID07, MIDM, 10, 16)
    DEP_FIELD(MASTER_ID07, MID, 10, 0)
DEP_REG32(MASTER_ID08, 0x120)
    DEP_FIELD(MASTER_ID08, MIDP, 1, 31)
    DEP_FIELD(MASTER_ID08, MIDR, 1, 30)
    DEP_FIELD(MASTER_ID08, MIDM, 10, 16)
    DEP_FIELD(MASTER_ID08, MID, 10, 0)
DEP_REG32(MASTER_ID09, 0x124)
    DEP_FIELD(MASTER_ID09, MIDP, 1, 31)
    DEP_FIELD(MASTER_ID09, MIDR, 1, 30)
    DEP_FIELD(MASTER_ID09, MIDM, 10, 16)
    DEP_FIELD(MASTER_ID09, MID, 10, 0)
DEP_REG32(MASTER_ID10, 0x128)
    DEP_FIELD(MASTER_ID10, MIDP, 1, 31)
    DEP_FIELD(MASTER_ID10, MIDR, 1, 30)
    DEP_FIELD(MASTER_ID10, MIDM, 10, 16)
    DEP_FIELD(MASTER_ID10, MID, 10, 0)
DEP_REG32(MASTER_ID11, 0x12c)
    DEP_FIELD(MASTER_ID11, MIDP, 1, 31)
    DEP_FIELD(MASTER_ID11, MIDR, 1, 30)
    DEP_FIELD(MASTER_ID11, MIDM, 10, 16)
    DEP_FIELD(MASTER_ID11, MID, 10, 0)
DEP_REG32(MASTER_ID12, 0x130)
    DEP_FIELD(MASTER_ID12, MIDP, 1, 31)
    DEP_FIELD(MASTER_ID12, MIDR, 1, 30)
    DEP_FIELD(MASTER_ID12, MIDM, 10, 16)
    DEP_FIELD(MASTER_ID12, MID, 10, 0)
DEP_REG32(MASTER_ID13, 0x134)
    DEP_FIELD(MASTER_ID13, MIDP, 1, 31)
    DEP_FIELD(MASTER_ID13, MIDR, 1, 30)
    DEP_FIELD(MASTER_ID13, MIDM, 10, 16)
    DEP_FIELD(MASTER_ID13, MID, 10, 0)
DEP_REG32(MASTER_ID14, 0x138)
    DEP_FIELD(MASTER_ID14, MIDP, 1, 31)
    DEP_FIELD(MASTER_ID14, MIDR, 1, 30)
    DEP_FIELD(MASTER_ID14, MIDM, 10, 16)
    DEP_FIELD(MASTER_ID14, MID, 10, 0)
DEP_REG32(MASTER_ID15, 0x13c)
    DEP_FIELD(MASTER_ID15, MIDP, 1, 31)
    DEP_FIELD(MASTER_ID15, MIDR, 1, 30)
    DEP_FIELD(MASTER_ID15, MIDM, 10, 16)
    DEP_FIELD(MASTER_ID15, MID, 10, 0)
DEP_REG32(MASTER_ID16, 0x140)
    DEP_FIELD(MASTER_ID16, MIDP, 1, 31)
    DEP_FIELD(MASTER_ID16, MIDR, 1, 30)
    DEP_FIELD(MASTER_ID16, MIDM, 10, 16)
    DEP_FIELD(MASTER_ID16, MID, 10, 0)
DEP_REG32(MASTER_ID17, 0x144)
    DEP_FIELD(MASTER_ID17, MIDP, 1, 31)
    DEP_FIELD(MASTER_ID17, MIDR, 1, 30)
    DEP_FIELD(MASTER_ID17, MIDM, 10, 16)
    DEP_FIELD(MASTER_ID17, MID, 10, 0)
DEP_REG32(MASTER_ID18, 0x148)
    DEP_FIELD(MASTER_ID18, MIDP, 1, 31)
    DEP_FIELD(MASTER_ID18, MIDR, 1, 30)
    DEP_FIELD(MASTER_ID18, MIDM, 10, 16)
    DEP_FIELD(MASTER_ID18, MID, 10, 0)
DEP_REG32(MASTER_ID19, 0x14c)
    DEP_FIELD(MASTER_ID19, MIDP, 1, 31)
    DEP_FIELD(MASTER_ID19, MIDR, 1, 30)
    DEP_FIELD(MASTER_ID19, MIDM, 10, 16)
    DEP_FIELD(MASTER_ID19, MID, 10, 0)
DEP_REG32(RAM_ADJ, 0x1fc)
    DEP_FIELD(RAM_ADJ, MESSAGE_EMAS, 1, 13)
    DEP_FIELD(RAM_ADJ, MESSAGE_EMAW, 2, 11)
    DEP_FIELD(RAM_ADJ, MESSAGE_EMA, 3, 8)
    DEP_FIELD(RAM_ADJ, PERMISSION_EMAS, 1, 5)
    DEP_FIELD(RAM_ADJ, PERMISSION_EMAW, 2, 3)
    DEP_FIELD(RAM_ADJ, PERMISSION_EMA, 3, 0)

#define R_MAX (R_RAM_ADJ + 1)

#define NR_MID_ENTRIES 20

#define NR_32B_APL_ENTRIES 128
#define NR_64K_APL_ENTRIES 256
#define NR_1M_APL_ENTRIES 16
#define NR_512M_APL_ENTRIES 1
#define NR_APL_ENTRIES (NR_32B_APL_ENTRIES + NR_64K_APL_ENTRIES \
                        + NR_1M_APL_ENTRIES + NR_512M_APL_ENTRIES)

typedef enum {
    GRANULE_32B,
    GRANULE_64K,
    GRANULE_1M,
    GRANULE_512M,
} XPPUGranule;

typedef struct XPPU XPPU;

typedef struct XPPUAperture {
    XPPU *parent;
    MemoryRegion iomem;

    XPPUGranule granule;
    /* MR base so we can offset the forwarded access.  */
    uint64_t base;
    /* Mask used to extract parts of the incoming address.  */
    uint64_t extract_mask;
    uint64_t extract_shift;
    /* RAM base. Start of APL tables for this particular Aperture.  */
    uint32_t ram_base;
} XPPUAperture;

struct XPPU {
    SysBusDevice parent_obj;
    MemoryRegion iomem;
    qemu_irq irq_isr;

    MemoryRegion *mr;
    AddressSpace *as;

    XPPUAperture ap[4];

    uint32_t perm_ram[NR_APL_ENTRIES];

    uint32_t regs[R_MAX];
    DepRegisterInfo regs_info[R_MAX];
};

static bool parity32(uint32_t v)
{
    /* We compute the parity in parallel, folding
     * each result in half, all the way down to one last bit.
     */
    v = v ^ (v >> 16);
    v = v ^ (v >> 8);
    v = v ^ (v >> 4);
    v = v ^ (v >> 2);
    v = v ^ (v >> 1);
    return v & 1;
}

static bool check_mid_parity(XPPU *s, uint32_t val32)
{
    static const uint32_t rsvd = 0x3c00fc00;
    bool p_written;
    bool p_computed;

    if (!DEP_AF_EX32(s->regs, CTRL, MID_PARITY_EN)) {
        return true;
    }

    /* Save the written parity.  */
    p_written = val32 & 0x80000000;

    /* Mask off parity field.  */
    val32 &= ~0x80000000;
    /* Mask off rsvd fields.  */
    val32 &= ~rsvd;
    p_computed = parity32(val32);

    return p_written == p_computed;
}

static void check_mid_parities(XPPU *s)
{
    unsigned int i;

    for (i = 0; i < NR_MID_ENTRIES; i++) {
        uint32_t val32;

        val32 = s->regs[R_MASTER_ID00 + i];

        /* Check MID parity.  */
        if (check_mid_parity(s, val32) == false) {
            DEP_AF_DP32(s->regs, ISR, MID_PARITY, true);
            continue;
        }
    }
}

static bool check_apl_parity(XPPU *s, uint32_t val32)
{
    unsigned int i;
    /* Bit 31 - Parity of 27, 19:15.
     * Bit 30 - Parity of 14:10
     * Bit 29 - Parity of 9:5
     * Bit 28 - Parity of 4:0
     */
    static const uint32_t apl_parities[] = {
        0x1f,
        0x1f << 5,
        0x1f << 10,
        (0x1f << 15) | 1 << 27,
    };
    uint32_t p = 0, p_written;
    bool ok;

    if (!DEP_AF_EX32(s->regs, CTRL, APER_PARITY_EN)) {
        return true;
    }

    for (i = 0; i < ARRAY_SIZE(apl_parities); i++) {
        uint32_t v = val32;
        bool parity;

        /* Extract the lanes for this pairity.  */
        v &= apl_parities[i];
        parity = parity32(v);
        p |= ((int) parity) << i;
    }

    p_written = val32 >> 28;
    ok = p_written == p;

    if (!ok) {
        qemu_log_mask(LOG_GUEST_ERROR, "Bad APL parity!\n");
        DEP_AF_DP32(s->regs, ISR, APER_PARITY, true);
    }
    return ok;
}

static void isr_update_irq(XPPU *s)
{
    bool pending = s->regs[R_ISR] & ~s->regs[R_IMR];
    qemu_set_irq(s->irq_isr, pending);
}

static void isr_postw(DepRegisterInfo *reg, uint64_t val64)
{
    XPPU *s = XILINX_XPPU(reg->opaque);
    isr_update_irq(s);
}

static uint64_t ien_prew(DepRegisterInfo *reg, uint64_t val64)
{
    XPPU *s = XILINX_XPPU(reg->opaque);
    uint32_t val = val64;

    s->regs[R_IMR] &= ~val;
    isr_update_irq(s);
    return 0;
}

static uint64_t ids_prew(DepRegisterInfo *reg, uint64_t val64)
{
    XPPU *s = XILINX_XPPU(reg->opaque);
    uint32_t val = val64;

    s->regs[R_IMR] |= val;
    isr_update_irq(s);
    return 0;
}

static void update_mrs(XPPU *s)
{
    bool xppu_enabled = DEP_AF_EX32(s->regs, CTRL, ENABLE);
    unsigned int i;

    for (i = 0; i < ARRAY_SIZE(s->ap); i++) {
        memory_region_set_enabled(&s->ap[i].iomem, xppu_enabled);
    }
}

static void ctrl_postw(DepRegisterInfo *reg, uint64_t val64)
{
    XPPU *s = XILINX_XPPU(reg->opaque);
    update_mrs(s);
    check_mid_parities(s);
    isr_update_irq(s);
}

static void mid_postw(DepRegisterInfo *reg, uint64_t val64)
{
    XPPU *s = XILINX_XPPU(reg->opaque);
    check_mid_parity(s, val64);
    isr_update_irq(s);
}

static DepRegisterAccessInfo xppu_regs_info[] = {
    {   .name = "CTRL",  .decode.addr = A_CTRL,
        .rsvd = 0xfffffff8,
        .ro = 0xfffffff8,
        .post_write = ctrl_postw,
    },{ .name = "ERR_STATUS1",  .decode.addr = A_ERR_STATUS1,
        .rsvd = 0xfff00000,
        .ro = 0xffffffff,
    },{ .name = "ERR_STATUS2",  .decode.addr = A_ERR_STATUS2,
        .rsvd = 0xfffffc00,
        .ro = 0xffffffff,
    },{ .name = "POISON",  .decode.addr = A_POISON,
        .reset = 0xff9c0,
        .rsvd = 0xfff00000,
        .ro = 0xffffffff,
    },{ .name = "ISR",  .decode.addr = A_ISR,
        .rsvd = 0xffffff10,
        .ro = 0xffffff10,
        .w1c = 0xef,
        .post_write = isr_postw,
    },{ .name = "IMR",  .decode.addr = A_IMR,
        .reset = 0xef,
        .rsvd = 0xffffff10,
        .ro = 0xffffffff,
    },{ .name = "IEN",  .decode.addr = A_IEN,
        .rsvd = 0xffffff10,
        .ro = 0xffffff10,
        .pre_write = ien_prew,
    },{ .name = "IDS",  .decode.addr = A_IDS,
        .rsvd = 0xffffff10,
        .ro = 0xffffff10,
        .pre_write = ids_prew,
    },{ .name = "M_MASTER_IDS",  .decode.addr = A_M_MASTER_IDS,
        .reset = 0x14,
        .ro = 0xffffffff,
    },{ .name = "M_APERTURE_32B",  .decode.addr = A_M_APERTURE_32B,
        .reset = 0x80,
        .ro = 0xffffffff,
    },{ .name = "M_APERTURE_64KB",  .decode.addr = A_M_APERTURE_64KB,
        .reset = 0x100,
        .ro = 0xffffffff,
    },{ .name = "M_APERTURE_1MB",  .decode.addr = A_M_APERTURE_1MB,
        .reset = 0x10,
        .ro = 0xffffffff,
    },{ .name = "M_APERTURE_512MB",  .decode.addr = A_M_APERTURE_512MB,
        .reset = 0x1,
        .ro = 0xffffffff,
    },{ .name = "BASE_32B",  .decode.addr = A_BASE_32B,
        .reset = 0xff990000,
        .ro = 0xffffffff,
    },{ .name = "BASE_64KB",  .decode.addr = A_BASE_64KB,
        .reset = 0xff000000,
        .ro = 0xffffffff,
    },{ .name = "BASE_1MB",  .decode.addr = A_BASE_1MB,
        .reset = 0xfe000000,
        .ro = 0xffffffff,
    },{ .name = "BASE_512MB",  .decode.addr = A_BASE_512MB,
        .reset = 0xc0000000,
        .ro = 0xffffffff,
    },{ .name = "ECO",  .decode.addr = A_ECO,
    },{ .name = "MASTER_ID00",  .decode.addr = A_MASTER_ID00,
        .reset = 0x83ff0040,
        .rsvd = 0x3c00fc00,
        .ro = 0x3c00fc00,
        .post_write = mid_postw,
    },{ .name = "MASTER_ID01",  .decode.addr = A_MASTER_ID01,
        .reset = 0x3f00000,
        .rsvd = 0x3c00fc00,
        .ro = 0x3c00fc00,
        .post_write = mid_postw,
    },{ .name = "MASTER_ID02",  .decode.addr = A_MASTER_ID02,
        .reset = 0x83f00010,
        .rsvd = 0x3c00fc00,
        .ro = 0x3c00fc00,
        .post_write = mid_postw,
    },{ .name = "MASTER_ID03",  .decode.addr = A_MASTER_ID03,
        .reset = 0x83c00080,
        .rsvd = 0x3c00fc00,
        .ro = 0x3c00fc00,
        .post_write = mid_postw,
    },{ .name = "MASTER_ID04",  .decode.addr = A_MASTER_ID04,
        .reset = 0x83c30080,
        .rsvd = 0x3c00fc00,
        .ro = 0x3c00fc00,
    },{ .name = "MASTER_ID05",  .decode.addr = A_MASTER_ID05,
        .reset = 0x3c30081,
        .rsvd = 0x3c00fc00,
        .ro = 0x3c00fc00,
    },{ .name = "MASTER_ID06",  .decode.addr = A_MASTER_ID06,
        .reset = 0x3c30082,
        .rsvd = 0x3c00fc00,
        .ro = 0x3c00fc00,
    },{ .name = "MASTER_ID07",  .decode.addr = A_MASTER_ID07,
        .reset = 0x83c30083,
        .rsvd = 0x3c00fc00,
        .ro = 0x3c00fc00,
    },{ .name = "MASTER_ID08",  .decode.addr = A_MASTER_ID08,
        .rsvd = 0x3c00fc00,
        .ro = 0x3c00fc00,
    },{ .name = "MASTER_ID09",  .decode.addr = A_MASTER_ID09,
        .rsvd = 0x3c00fc00,
        .ro = 0x3c00fc00,
    },{ .name = "MASTER_ID10",  .decode.addr = A_MASTER_ID10,
        .rsvd = 0x3c00fc00,
        .ro = 0x3c00fc00,
    },{ .name = "MASTER_ID11",  .decode.addr = A_MASTER_ID11,
        .rsvd = 0x3c00fc00,
        .ro = 0x3c00fc00,
    },{ .name = "MASTER_ID12",  .decode.addr = A_MASTER_ID12,
        .rsvd = 0x3c00fc00,
        .ro = 0x3c00fc00,
    },{ .name = "MASTER_ID13",  .decode.addr = A_MASTER_ID13,
        .rsvd = 0x3c00fc00,
        .ro = 0x3c00fc00,
    },{ .name = "MASTER_ID14",  .decode.addr = A_MASTER_ID14,
        .rsvd = 0x3c00fc00,
        .ro = 0x3c00fc00,
    },{ .name = "MASTER_ID15",  .decode.addr = A_MASTER_ID15,
        .rsvd = 0x3c00fc00,
        .ro = 0x3c00fc00,
    },{ .name = "MASTER_ID16",  .decode.addr = A_MASTER_ID16,
        .rsvd = 0x3c00fc00,
        .ro = 0x3c00fc00,
    },{ .name = "MASTER_ID17",  .decode.addr = A_MASTER_ID17,
        .rsvd = 0x3c00fc00,
        .ro = 0x3c00fc00,
    },{ .name = "MASTER_ID18",  .decode.addr = A_MASTER_ID18,
        .rsvd = 0x3c00fc00,
        .ro = 0x3c00fc00,
    },{ .name = "MASTER_ID19",  .decode.addr = A_MASTER_ID19,
        .rsvd = 0x3c00fc00,
        .ro = 0x3c00fc00,
    },{ .name = "RAM_ADJ",  .decode.addr = A_RAM_ADJ,
        .reset = 0xb0b,
        .rsvd = 0xffffc0c0,
        .ro = 0xffffc0c0,
    }
};

static void xppu_reset(DeviceState *dev)
{
    XPPU *s = XILINX_XPPU(dev);
    unsigned int i;

    for (i = 0; i < ARRAY_SIZE(s->regs_info); ++i) {
        dep_register_reset(&s->regs_info[i]);
    }
    update_mrs(s);
    isr_update_irq(s);
}

static bool xppu_ap_check(XPPU *s, MemoryTransaction *tr, uint32_t apl)
{
    unsigned int i;
    bool mid_match = false;
    bool tz = extract32(apl, 27, 1);
    bool ok;

    ok = check_apl_parity(s, apl);
    if (!ok) {
        return false;
    }

    /* Check MIDs.  */
    for (i = 0; i < NR_MID_ENTRIES; i++) {
        uint32_t val32, mid, mask;
        bool readonly;

        if (!extract32(apl, i, 1)) {
            continue;
        }

        val32 = s->regs[R_MASTER_ID00 + i];
        mid = DEP_F_EX32(val32, MASTER_ID00, MID);
        readonly = DEP_F_EX32(val32, MASTER_ID00, MIDR);
        mask = DEP_F_EX32(val32, MASTER_ID00, MIDM);

        if ((mid & mask) != (tr->attr.master_id & mask)) {
            continue;
        }

        mid_match = true;

        /* Check MID parity.  */
        if (check_mid_parity(s, val32) == false) {
            DEP_AF_DP32(s->regs, ISR, MID_PARITY, true);
            continue;
        }

        if (readonly && tr->rw) {
            DEP_AF_DP32(s->regs, ISR, MID_RO, true);
            continue;
        }

        if (!tr->attr.secure && !tz) {
            DEP_AF_DP32(s->regs, ISR, APER_TZ, true);
            continue;
        }

        break;
    }

    if (!mid_match) {
        /* Set if MID checks don't make it past masking and compare.  */
        DEP_AF_DP32(s->regs, ISR, MID_MISS, true);
    }

    return i < NR_MID_ENTRIES;
}

static void xppu_ap_access(MemoryTransaction *tr)
{
    XPPUAperture *ap = tr->opaque;
    XPPU *s = ap->parent;
    hwaddr addr = tr->addr;
    uint32_t ram_offset;
    uint32_t apl;
    bool valid;
    bool isr_free;
    bool xppu_enabled = DEP_AF_EX32(s->regs, CTRL, ENABLE);

    assert(xppu_enabled);

    addr += ap->base;

    /* If any of bits ISR[7:1] are set, we cant store new faults.  */
    isr_free = (s->regs[R_ISR] & 0xf6) == 0;

    ram_offset = addr & ap->extract_mask;
    ram_offset >>= ap->extract_shift;

    ram_offset += ap->ram_base;
    apl = s->perm_ram[ram_offset];
    valid = xppu_ap_check(s, tr, apl);

    if (!valid) {
        if (isr_free) {
            DEP_AF_DP32(s->regs, ISR, APER_PERM, true);
            DEP_AF_DP32(s->regs, ERR_STATUS1, AXI_ADDR, addr >> 12);
            DEP_AF_DP32(s->regs, ERR_STATUS2, AXI_ID, tr->attr.master_id);
        }

        /* Poison the transaction.
         *
         * Bits 11:0  remain untouched.
         * Bits 31:12 are taken from the POISONBASE register
         * Bits 48:32 are zeroed.
         */
        addr &= (1 << 12) - 1;
        addr |= DEP_AF_EX32(s->regs, POISON, BASE) << 12;
        isr_update_irq(s);
    }

    /* The access is accepted, let it through.  */
    tr->data.u64 = cpu_to_le64(tr->data.u64);
    address_space_rw(s->as, addr, tr->attr, (uint8_t *) &tr->data.u64,
                          tr->size, tr->rw);
    tr->data.u64 = le64_to_cpu(tr->data.u64);
}

static const MemoryRegionOps xppu_ap_ops = {
    .access = xppu_ap_access,
    .endianness = DEVICE_LITTLE_ENDIAN,
    .valid = {
        .min_access_size = 4,
        .max_access_size = 4,
    },
};

static uint64_t xppu_read(void *opaque, hwaddr addr, unsigned size,
                          MemTxAttrs attr)
{
    XPPU *s = XILINX_XPPU(opaque);
    DepRegisterInfo *r;

    if (!attr.secure) {
        /* Non secure, return zero */
        return 0;
    }

    if (addr >= 0x1000 && addr < 0x1644) {
        unsigned int i = (addr - 0x1000) / 4;
        assert(i < ARRAY_SIZE(s->perm_ram));
        return s->perm_ram[i];
    }

    /* We are reading off the end of the defined registers, but still in the
     * XPPU memory space.
     */
    if ((addr / 4) > R_MAX) {
        qemu_log_mask(LOG_GUEST_ERROR, "%s: Decode error: "
                      "read from %" HWADDR_PRIx "\n",
                      object_get_canonical_path(OBJECT(s)),
                      addr);
        return 0;
    }

    r = &s->regs_info[addr / 4];

    if (!r->data) {
        qemu_log("%s: Decode error: read from %" HWADDR_PRIx "\n",
                 object_get_canonical_path(OBJECT(s)),
                 addr);
        DEP_AF_DP32(s->regs, ISR, INV_APB, true);
        isr_update_irq(s);
        return 0;
    }
    return dep_register_read(r);
}

static void xppu_write(void *opaque, hwaddr addr, uint64_t value,
                       unsigned size, MemTxAttrs attr)
{
    XPPU *s = XILINX_XPPU(opaque);
    DepRegisterInfo *r;

    if (!attr.secure) {
        return;
    }

    if (addr >= 0x1000 && addr < 0x1644) {
        unsigned int i = (addr - 0x1000) / 4;
        assert(i < ARRAY_SIZE(s->perm_ram));
        s->perm_ram[i] = value;
        return;
    }

    /* We are writing off the end of the defined registers, but still in the
     * XPPU memory space.
     */
    if ((addr / 4) > R_MAX) {
        qemu_log_mask(LOG_GUEST_ERROR, "%s: Decode error: "
                      "write to %" HWADDR_PRIx "=%" PRIx64 "\n",
                      object_get_canonical_path(OBJECT(s)),
                      addr, value);
        return;
    }

    r = &s->regs_info[addr / 4];

    if (!r->data) {
        qemu_log("%s: Decode error: write to %" HWADDR_PRIx "=%" PRIx64 "\n",
                 object_get_canonical_path(OBJECT(s)),
                 addr, value);
        DEP_AF_DP32(s->regs, ISR, INV_APB, true);
        isr_update_irq(s);
        return;
    }
    dep_register_write(r, value, ~0);
}

static void xppu_access(MemoryTransaction *tr)
{
    MemTxAttrs attr = tr->attr;
    void *opaque = tr->opaque;
    hwaddr addr = tr->addr;
    unsigned size = tr->size;
    uint64_t value = tr->data.u64;;
    bool is_write = tr->rw;

    if (is_write) {
        xppu_write(opaque, addr, value, size, attr);
    } else {
        tr->data.u64 = xppu_read(opaque, addr, size, attr);
    }
}

static const MemoryRegionOps xppu_ops = {
    .access = xppu_access,
    .endianness = DEVICE_LITTLE_ENDIAN,
    .valid = {
        .min_access_size = 4,
        .max_access_size = 4,
    },
};

static void xppu_realize(DeviceState *dev, Error **errp)
{
    XPPU *s = XILINX_XPPU(dev);
    const char *prefix = object_get_canonical_path(OBJECT(dev));
    unsigned int i;

    for (i = 0; i < ARRAY_SIZE(xppu_regs_info); ++i) {
        DepRegisterInfo *r = &s->regs_info[xppu_regs_info[i].decode.addr/4];

        *r = (DepRegisterInfo) {
            .data = (uint8_t *)&s->regs[
                    xppu_regs_info[i].decode.addr/4],
            .data_size = sizeof(uint32_t),
            .access = &xppu_regs_info[i],
            .debug = XILINX_XPPU_ERR_DEBUG,
            .prefix = prefix,
            .opaque = s,
        };
    }

    s->as = s->mr ? address_space_init_shareable(s->mr, NULL)
                          : &address_space_memory;
}

static void xppu_init(Object *obj)
{
    XPPU *s = XILINX_XPPU(obj);
    SysBusDevice *sbd = SYS_BUS_DEVICE(obj);

    sysbus_init_irq(sbd, &s->irq_isr);

    object_property_add_link(obj, "mr", TYPE_MEMORY_REGION,
                             (Object **)&s->mr,
                             qdev_prop_allow_set_link_before_realize,
                             OBJ_PROP_LINK_STRONG,
                             &error_abort);
}

static bool xppu_parse_reg(FDTGenericMMap *obj, FDTGenericRegPropInfo reg,
                           Error **errp)
{
    XPPU *s = XILINX_XPPU(obj);
    SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
    ObjectClass *klass = object_class_by_name(TYPE_XILINX_XPPU);
    FDTGenericMMapClass *parent_fmc;
    unsigned int i;

    parent_fmc = FDT_GENERIC_MMAP_CLASS(object_class_get_parent(klass));
    memory_region_init_io(&s->iomem, OBJECT(obj), &xppu_ops, s,
                          TYPE_XILINX_XPPU, 0x10000);
    sysbus_init_mmio(sbd, &s->iomem);

    for (i = 0; i < (reg.n - 1) && i < ARRAY_SIZE(s->ap); i++) {
        static const XPPUGranule granules[] = {
            GRANULE_32B,
            GRANULE_64K,
            GRANULE_1M,
            GRANULE_512M
        };
        static const uint64_t bases[] = {
            0xff990000,
            0xff000000,
            0xfe000000,
            0xc0000000,
        };
        static const uint64_t masks[] = {
            0x7f << 5,  /* 32B, bits 11:05.  */
            0xff << 16, /* 64K, bits 23:16.  */
            0x0f << 20, /* 1MB, bits 23:20.  */
            0, /* No extraction.  */
        };
        static const unsigned int shifts[] = {
            5,  /* 32B, bits 11:05.  */
            16, /* 64K, bits 23:16.  */
            20, /* 1MB, bits 23:20.  */
            0, /* No extraction.  */
        };
        static const uint32_t ram_bases[] = {
            0x100,
            0x0,
            0x180,
            0x190,
        };
        char *name;

        s->ap[i].parent = s;
        s->ap[i].granule = granules[i];
        s->ap[i].base = bases[i];
        s->ap[i].extract_mask = masks[i];
        s->ap[i].extract_shift = shifts[i];
        s->ap[i].ram_base = ram_bases[i];

        name = g_strdup_printf("xppu-mr-%d\n", i);
        memory_region_init_io(&s->ap[i].iomem, OBJECT(obj),
                              &xppu_ap_ops, &s->ap[i],
                              name, reg.s[i + 1]);
        sysbus_init_mmio(sbd, &s->ap[i].iomem);
        g_free(name);
    }
    return parent_fmc ? parent_fmc->parse_reg(obj, reg, errp) : false;
}

static const VMStateDescription vmstate_xppu = {
    .name = TYPE_XILINX_XPPU,
    .version_id = 1,
    .minimum_version_id = 1,
    .minimum_version_id_old = 1,
    .fields = (VMStateField[]) {
        VMSTATE_UINT32_ARRAY(regs, XPPU, R_MAX),
        VMSTATE_END_OF_LIST(),
    }
};

static void xppu_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    FDTGenericMMapClass *fmc = FDT_GENERIC_MMAP_CLASS(klass);

    dc->reset = xppu_reset;
    dc->realize = xppu_realize;
    dc->vmsd = &vmstate_xppu;
    fmc->parse_reg = xppu_parse_reg;
}

static const TypeInfo xppu_info = {
    .name          = TYPE_XILINX_XPPU,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(XPPU),
    .class_init    = xppu_class_init,
    .instance_init = xppu_init,
    .interfaces    = (InterfaceInfo[]) {
        { TYPE_FDT_GENERIC_MMAP },
        { },
    },

};

static void xppu_register_types(void)
{
    type_register_static(&xppu_info);
}

type_init(xppu_register_types)
