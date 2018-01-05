/*
 * Xilinx XIIC I2C Controller
 *
 * Copyright (c) 2006-2007 CodeSourcery.
 * Copyright (c) 2012 Oskar Andero <oskar.andero@gmail.com>
 * Copyright (c) 2017 Biamp Systems Corporation
 *
 * This file is derived from hw/realview.c by Paul Brook
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "qemu/osdep.h"
#include "qemu-common.h"
#include "hw/sysbus.h"
#include "qemu/log.h"


#define XIIC_MSB_OFFSET 0

/* Register Offsets */
#define XIIC_REG_OFFSET      (0x100+XIIC_MSB_OFFSET)
#define XIIC_CR_REG_OFFSET   (0x00+XIIC_REG_OFFSET)     /* Control Register   */
#define XIIC_SR_REG_OFFSET   (0x04+XIIC_REG_OFFSET)     /* Status Register    */
#define XIIC_DTR_REG_OFFSET  (0x08+XIIC_REG_OFFSET)     /* Data Tx Register   */
#define XIIC_DRR_REG_OFFSET  (0x0C+XIIC_REG_OFFSET)     /* Data Rx Register   */
#define XIIC_ADR_REG_OFFSET  (0x10+XIIC_REG_OFFSET)     /* Address Register   */
#define XIIC_TFO_REG_OFFSET  (0x14+XIIC_REG_OFFSET)     /* Tx FIFO Occupancy  */
#define XIIC_RFO_REG_OFFSET  (0x18+XIIC_REG_OFFSET)     /* Rx FIFO Occupancy  */
#define XIIC_TBA_REG_OFFSET  (0x1C+XIIC_REG_OFFSET)     /* 10 Bit Address reg */
#define XIIC_RFD_REG_OFFSET  (0x20+XIIC_REG_OFFSET)     /* Rx FIFO Depth reg  */
#define XIIC_GPO_REG_OFFSET  (0x24+XIIC_REG_OFFSET)     /* Output Register    */

/* Control Register masks */
#define XIIC_CR_ENABLE_DEVICE_MASK        0x01          /* Device enable = 1      */
#define XIIC_CR_TX_FIFO_RESET_MASK        0x02          /* Transmit FIFO reset=1  */
#define XIIC_CR_MSMS_MASK                 0x04          /* Master starts Txing=1  */
#define XIIC_CR_DIR_IS_TX_MASK            0x08          /* Dir of tx. Txing=1     */
#define XIIC_CR_NO_ACK_MASK               0x10          /* Tx Ack. NO ack = 1     */
#define XIIC_CR_REPEATED_START_MASK       0x20          /* Repeated start = 1     */
#define XIIC_CR_GENERAL_CALL_MASK         0x40          /* Gen Call enabled = 1   */

/* Status Register masks */
#define XIIC_SR_GEN_CALL_MASK             0x01          /* 1=a mstr issued a GC   */
#define XIIC_SR_ADDR_AS_SLAVE_MASK        0x02          /* 1=when addr as slave   */
#define XIIC_SR_BUS_BUSY_MASK             0x04          /* 1 = bus is busy        */
#define XIIC_SR_MSTR_RDING_SLAVE_MASK     0x08          /* 1=Dir: mstr <-- slave  */
#define XIIC_SR_TX_FIFO_FULL_MASK         0x10          /* 1 = Tx FIFO full       */
#define XIIC_SR_RX_FIFO_FULL_MASK         0x20          /* 1 = Rx FIFO full       */
#define XIIC_SR_RX_FIFO_EMPTY_MASK        0x40          /* 1 = Rx FIFO empty      */
#define XIIC_SR_TX_FIFO_EMPTY_MASK        0x80          /* 1 = Tx FIFO empty      */

/*
 * The following constants define the register offsets for the Interrupt
 * registers. There are some holes in the memory map for reserved addresses
 * to allow other registers to be added and still match the memory map of the
 * interrupt controller registers
 */
#define XIIC_DGIER_OFFSET                 0x1C /* Device Global Interrupt Enable Register */
#define XIIC_IISR_OFFSET                  0x20 /* Interrupt Status Register */
#define XIIC_IIER_OFFSET                  0x28 /* Interrupt Enable Register */
#define XIIC_RESETR_OFFSET                0x40 /* Reset Register */

#define XIIC_RESET_MASK                   0xAUL

#define TYPE_XIIC_I2C "xlnx,xps-iic-2.00.a"
#define XIIC_I2C(obj) \
    OBJECT_CHECK(xiic_i2c_state, (obj), TYPE_XIIC_I2C)

typedef struct xiic_i2c_state {
    SysBusDevice busDev;
    MemoryRegion memRegion;
    uint32_t baseAddress;
} xiic_i2c_state;

static uint64_t xiic_i2c_read(void *opaque, hwaddr offset,
                                   unsigned size)
{
    //xiic_i2c_state *s = (xiic_i2c_state *)opaque;

    qemu_log("xiic_i2c_read at offset %lX\n", offset);

    switch(offset) {
    case XIIC_SR_REG_OFFSET:
        return XIIC_SR_RX_FIFO_EMPTY_MASK | XIIC_SR_TX_FIFO_EMPTY_MASK;
        break;

    default:
        return 0;
    }
}

static void xiic_i2c_write(void *opaque, hwaddr offset,
                                uint64_t value, unsigned size)
{
//    xiic_i2c_state *s = (xiic_i2c_state *)opaque;
    qemu_log("xiic_i2c_write at offset 0x%lX value 0x%llX\n", offset, value);
//    qemu_log("xiic_i2c_write at offset %lX\n", offset);
//
//    switch (offset) {
//    case 0:
//        s->out |= value & 3;
//        break;
//    case 4:
//        s->out &= ~value;
//        break;
//    default:
//        qemu_log_mask(LOG_GUEST_ERROR,
//                      "%s: Bad offset 0x%x\n", __func__, (int)offset);
//    }
//    bitbang_i2c_set(s->bitbang, BITBANG_I2C_SCL, (s->out & 1) != 0);
//    s->in = bitbang_i2c_set(s->bitbang, BITBANG_I2C_SDA, (s->out & 2) != 0);
}

static const MemoryRegionOps xiic_i2c_ops = {
    .read = xiic_i2c_read,
    .write = xiic_i2c_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
};

static int xiic_i2c_init(SysBusDevice *dev)
{
    xiic_i2c_state *p = XIIC_I2C(dev);

    memory_region_init_io(&p->memRegion, 
                          OBJECT(p), 
                          &xiic_i2c_ops, 
                          p,
                          "xiic_i2c", 
                          0x10000);

    sysbus_init_mmio(dev, &p->memRegion);
    return 0;
}

static Property xiic_i2c_properties[] = {
    DEFINE_PROP_UINT32("reg", xiic_i2c_state, baseAddress, 0),
    DEFINE_PROP_END_OF_LIST(),
};

static void xiic_i2c_class_init(ObjectClass *klass, void *data){
    DeviceClass *dc = DEVICE_CLASS(klass);
    SysBusDeviceClass *k = SYS_BUS_DEVICE_CLASS(klass);
    k->init = xiic_i2c_init;
    dc->props = xiic_i2c_properties;
}

static const TypeInfo xiic_i2c_info = {
    .name          = TYPE_XIIC_I2C,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(xiic_i2c_state),
    .class_init    = xiic_i2c_class_init,
};

static void xiic_i2c_register(void)
{
    type_register_static(&xiic_i2c_info);
}

type_init(xiic_i2c_register)
