#include "blockdev.h"
#include "hw.h"
#include "flash.h"
#include "pc.h"
#include "exec-memory.h"

#include "fdt_generic_util.h"
#include "fdt_generic_devices.h"

#define FLASH_SECTOR_SIZE (64 * 1024)

/* FIXME: This file should go away. When these devices are properly QOMified
 * then these FDT creations should happen automatically without need for these
 * explict shim functions
 */

int pflash_cfi01_fdt_init(char *node_path, FDTMachineInfo *fdti, void *opaque)
{

    int flash_base = 0;
    int flash_size = 0;
    Error *errp = NULL;

    int be = *((int *)opaque);

    DriveInfo *dinfo;
    int bank_width;

    flash_base = qemu_devtree_getprop_cell(fdti->fdt, node_path, "reg", 0,
                                                false, &errp);
    flash_size = qemu_devtree_getprop_cell(fdti->fdt, node_path, "reg", 1,
                                                false, &errp);
    bank_width = qemu_devtree_getprop_cell(fdti->fdt, node_path, "bank-width",
                                                0, false, &errp);
    assert_no_error(errp);

    printf("FDT: FLASH: baseaddr: 0x%x, size: 0x%x\n",
           flash_base, flash_size);

    dinfo = drive_get_next(IF_PFLASH);
    pflash_cfi01_register(flash_base, NULL, node_path, flash_size,
                            dinfo ? dinfo->bdrv : NULL, FLASH_SECTOR_SIZE,
                            flash_size/FLASH_SECTOR_SIZE,
                            bank_width, 0x89, 0x18, 0x0000, 0x0, be);
    return 0;
}

static int uart16550_fdt_init(char *node_path, FDTMachineInfo *fdti,
    void *priv)
{
    /* FIXME: Pass in dynamically */
    MemoryRegion *address_space_mem = get_system_memory();
    target_phys_addr_t base;
    int baudrate;
    qemu_irq irqline;
    char irq_info[1024];
    Error *errp = NULL;

    base = qemu_devtree_getprop_cell(fdti->fdt, node_path, "reg", 0,
                                        false, &errp);
    base += qemu_devtree_getprop_cell(fdti->fdt, node_path, "reg-offset", 0,
                                        false, &errp);
    assert_no_error(errp);
    base &= ~3ULL; /* qemu uart16550 model starts with 3* 8bit offset */

    baudrate = qemu_devtree_getprop_cell(fdti->fdt, node_path, "current-speed",
                                            0, false, &errp);
    if (errp) {
        baudrate = 115200;
    }

    irqline = fdt_get_irq_info(fdti, node_path, 0 , NULL, irq_info);
    printf("FDT: UART16550a: baseaddr: 0x"
           TARGET_FMT_plx ", irq: %s, baud %d\n", base, irq_info, baudrate);

    /* it_shift = 2, reg-shift in DTS - for Xilnx IP is hardcoded */
    (void) serial_mm_init(address_space_mem, base, 2, irqline, baudrate,
                            qemu_char_get_next_serial(), DEVICE_LITTLE_ENDIAN);
    return 0;
}

fdt_register_compatibility_n(uart16550_fdt_init, "ns16550", 0);
fdt_register_compatibility_n(uart16550_fdt_init, "ns16550a", 1);
