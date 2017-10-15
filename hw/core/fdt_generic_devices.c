#include "qemu/osdep.h"
#include "sysemu/blockdev.h"
#include "sysemu/char.h"
#include "hw/hw.h"
#include "hw/block/flash.h"
#include "hw/i386/pc.h"
#include "exec/memory.h"
#include "exec/address-spaces.h"
#include "qemu/log.h"
#include "hw/loader.h"
#include "hw/char/serial.h"
#include "qapi/error.h"

#include "hw/fdt_generic_util.h"
#include "hw/fdt_generic_devices.h"

#define FLASH_SECTOR_SIZE (64 * 1024)

/* FIXME: This file should go away. When these devices are properly QOMified
 * then these FDT creations should happen automatically without need for these
 * explict shim functions
 */

/* Piggy back fdt_generic_util.c ERR_DEBUG symbol as these two are really the
 * same feature
 */

#ifndef FDT_GENERIC_UTIL_ERR_DEBUG
#define FDT_GENERIC_UTIL_ERR_DEBUG 0
#endif
#define DB_PRINT(lvl, ...) do { \
    if (FDT_GENERIC_UTIL_ERR_DEBUG > (lvl)) { \
        qemu_log_mask(lvl, ": %s: ", __func__); \
        qemu_log_mask(lvl, ## __VA_ARGS__); \
    } \
} while (0);

#define DB_PRINT_NP(lvl, ...) do { \
    if (FDT_GENERIC_UTIL_ERR_DEBUG > (lvl)) { \
        qemu_log_mask(lvl, "%s", node_path); \
        DB_PRINT((lvl), ## __VA_ARGS__); \
    } \
} while (0);

int fdt_generic_num_cpus;

int pflash_cfi01_fdt_init(char *node_path, FDTMachineInfo *fdti, void *opaque)
{

    hwaddr flash_base = 0;
    uint32_t flash_size = 0;

    int be = *((int *)opaque);

    DriveInfo *dinfo;
    uint32_t bank_width;

    /* FIXME: respect #address and size cells */
    flash_base = qemu_fdt_getprop_cell(fdti->fdt, node_path, "reg", NULL, 0,
                                       false, &error_abort);
    flash_base += fdt_get_parent_base(node_path, fdti);
    flash_size = qemu_fdt_getprop_cell(fdti->fdt, node_path, "reg", NULL, 1,
                                       false, &error_abort);
    bank_width = qemu_fdt_getprop_cell(fdti->fdt, node_path, "bank-width",
                                       NULL, 0, false, &error_abort);

    DB_PRINT_NP(0, "FDT: FLASH: baseaddr: 0x%"HWADDR_PRIX", size: 0x%x\n",
                flash_base, flash_size);

    dinfo = drive_get_next(IF_PFLASH);
    pflash_cfi01_register(flash_base, NULL, node_path, flash_size,
                          dinfo ? blk_by_legacy_dinfo(dinfo) : NULL,
                          FLASH_SECTOR_SIZE,
                          flash_size/FLASH_SECTOR_SIZE,
                          bank_width, 0x89, 0x18, 0x0000, 0x0, be);
    return 0;
}

static int uart16550_fdt_init(char *node_path, FDTMachineInfo *fdti,
    void *priv)
{
    /* FIXME: Pass in dynamically */
    MemoryRegion *address_space_mem = get_system_memory();
    hwaddr base;
    uint32_t baudrate;
    qemu_irq irqline;
    bool map_mode;
    char irq_info[1024];
    Error *err = NULL;

    /* FIXME: respect #address and size cells */
    base = qemu_fdt_getprop_cell(fdti->fdt, node_path, "reg", NULL, 0,
                                 false, &error_abort);
    base += qemu_fdt_getprop_cell(fdti->fdt, node_path, "reg-offset", NULL, 0,
                                  false, &error_abort);
    base &= ~3ULL; /* qemu uart16550 model starts with 3* 8bit offset */

    baudrate = qemu_fdt_getprop_cell(fdti->fdt, node_path, "current-speed",
                                     NULL, 0, false, &err);
    if (err) {
        baudrate = 115200;
    }

    irqline = *fdt_get_irq_info(fdti, node_path, 0, irq_info, &map_mode);
    assert(!map_mode);
    DB_PRINT_NP(0, "FDT: UART16550a: baseaddr: 0x" TARGET_FMT_plx
                ", irq: %s, baud %d\n", base, irq_info, baudrate);

    /* it_shift = 2, reg-shift in DTS - for Xilnx IP is hardcoded */
    serial_mm_init(address_space_mem, base, 2, irqline, baudrate,
                   serial_hds[fdt_serial_ports], DEVICE_LITTLE_ENDIAN);
    return 0;
}

fdt_register_compatibility_n(uart16550_fdt_init, "ns16550", 0);
fdt_register_compatibility_n(uart16550_fdt_init, "ns16550a", 1);
