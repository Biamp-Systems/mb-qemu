/*
 * Utility functions for fdt generic framework
 *
 * Copyright (c) 2009 Edgar E. Iglesias.
 * Copyright (c) 2009 Michal Simek.
 * Copyright (c) 2011 PetaLogix Qld Pty Ltd.
 * Copyright (c) 2011 Peter A. G. Crosthwaite <peter.crosthwaite@petalogix.com>.
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

#define FDT_GENERIC_UTIL_ERR_DEBUG

#ifdef FDT_GENERIC_UTIL_ERR_DEBUG
#define DB_PRINT(...) do { \
    fprintf(stderr,  ": %s: ", __func__); \
    fprintf(stderr, ## __VA_ARGS__); \
    } while (0);
#else
    #define DB_PRINT(...)
#endif

#include "fdt_generic_util.h"
#include "net.h"

/* FIXME: wrap direct calls into libfdt */

#include <libfdt.h>

FDTMachineInfo *fdt_generic_create_machine(void *fdt, qemu_irq *cpu_irq)
{
    char node_path[DT_PATH_LENGTH];

    FDTMachineInfo *fdti = fdt_init_new_fdti(fdt);

    fdti->irq_base = cpu_irq;

    /* bind any force bound instances */
    fdt_force_bind_all(fdti);

    /* parse the device tree */
    if (!qemu_devtree_get_root_node(fdt, node_path)) {
        simple_bus_fdt_init(node_path, fdti, NULL);
        while (qemu_co_queue_enter_next(fdti->cq));
    } else {
        fprintf(stderr, "FDT: ERROR: cannot get root node from device tree %s\n"
            , node_path);
    }

    printf("FDT: Device tree scan complete\n");
    FDTMachineInfo *ret = g_malloc0(sizeof(*ret));
    return fdti;
}

struct FDTInitNodeArgs {
    char *node_path;
    FDTMachineInfo *fdti;
};

static int fdt_init_qdev(char *node_path, FDTMachineInfo *fdti, char *compat);

static void fdt_init_node(void *args)
{

    struct FDTInitNodeArgs *a = args;
    char *node_path = a->node_path;
    FDTMachineInfo *fdti = a->fdti;
    g_free(a);

    char *all_compats = NULL, *compat, *node_name, *next_compat;
    int compat_len;

    static int entry_index;
    int this_entry = entry_index++;
    DB_PRINT("enter %d %s\n", this_entry, node_path);

    /* try instance binding first */
    node_name = qemu_devtree_get_node_name(fdti->fdt, node_path);
    if (!node_name) {
        printf("FDT: ERROR: nameless node: %s\n", node_path);
    }
    if (!fdt_init_inst_bind(node_path, fdti, node_name)) {
        goto exit;
    }

    /* fallback to compatibility binding */
    all_compats = qemu_devtree_getprop(fdti->fdt, node_path,
        "compatible", &compat_len, false, NULL);
    if (!all_compats) {
        printf("FDT: ERROR: no compatibility found for node %s%s\n", node_path,
            node_name);
        DB_PRINT("exit %d\n", this_entry);
        return;
    }
    compat = all_compats;

try_next_compat:
    if (compat_len == 0) {
        goto invalidate;
    }
    if (!fdt_init_compat(node_path, fdti, compat)) {
        goto exit;
    }
    if (!fdt_init_qdev(node_path, fdti, compat)) {
        goto exit;
    }
    next_compat = rawmemchr(compat, '\0');
    compat_len -= (next_compat + 1 - compat);
    if (compat_len > 0) {
        *next_compat = ' ';
    }
    compat = next_compat+1;
    goto try_next_compat;
invalidate:
    printf("FDT: Unsupported peripheral invalidated %s compatibilities %s\n",
        node_name, all_compats);
    qemu_devtree_setprop_string(fdti->fdt, node_path, "compatible",
        "invalidated");
exit:

    DB_PRINT("exit %d\n", this_entry);

    if (!fdt_init_has_opaque(fdti, node_path)) {
        fdt_init_set_opaque(fdti, node_path, NULL);
    }
    g_free(node_path);
    g_free(all_compats);
    return;
}

int simple_bus_fdt_init(char *bus_node_path, FDTMachineInfo *fdti, void *unused)
{
    int i;
    int num_children = qemu_devtree_get_num_children(fdti->fdt, bus_node_path,
                                                        1);
    char **children = qemu_devtree_get_children(fdti->fdt, bus_node_path, 1);

    DB_PRINT("num child devices: %d\n", num_children);

    for (i = 0; i < num_children; i++) {
        struct FDTInitNodeArgs *init_args = g_malloc0(sizeof(*init_args));
        init_args->node_path = children[i];
        init_args->fdti = fdti;
        qemu_coroutine_enter(qemu_coroutine_create(fdt_init_node), init_args);
    }

    g_free(children);
    return 0;
}

qemu_irq fdt_get_irq_info(FDTMachineInfo *fdti, char *node_path, int irq_idx,
        int *err, char *info) {
    void *fdt = fdti->fdt;
    int intc_phandle, intc_cells, idx, errl;
    char intc_node_path[DT_PATH_LENGTH];
    Error *errp = NULL;
    DeviceState *intc;

    if (!err) {
        err = &errl;
    }
    intc_phandle = qemu_devtree_getprop_cell(fdt, node_path, "interrupt-parent",
                                                                0, true, &errp);
    if (errp) {
        goto fail;
    }

    if (qemu_devtree_get_node_by_phandle(fdt, intc_node_path, intc_phandle)) {
        goto fail;
    }
    intc_cells = qemu_devtree_getprop_cell(fdt, intc_node_path,
                                           "#interrupt-cells", 0, false, &errp);
    if (errp) {
        goto fail;
    }
    idx = qemu_devtree_getprop_cell(fdt, node_path, "interrupts",
                                        intc_cells * irq_idx, false, &errp);
    if (errp) {
        goto fail;
    }

    while (!fdt_init_has_opaque(fdti, intc_node_path)) {
        fdt_init_yield(fdti);
    }
    intc = DEVICE(fdt_init_get_opaque(fdti, intc_node_path));
    if (!intc) {
        goto fail;
    }
    if (info) {
        char *node_name = qemu_devtree_get_node_name(fdt, intc_node_path);
        sprintf(info, "%d (%s)", idx, node_name);
        g_free((void *)node_name);
    }
    return qdev_get_gpio_in(intc, idx);
fail:
    *err = 1;
    if (info) {
        sprintf(info, "(none)");
    }
    return NULL;
}

qemu_irq fdt_get_irq(FDTMachineInfo *fdti, char *node_path, int irq_idx)
{
    return fdt_get_irq_info(fdti, node_path, irq_idx, NULL, NULL);
}

/* FIXME: figure out a real solution to this */

#define DIGIT(a) ((a) >= '0' && (a) <= '9')
#define LOWER_CASE(a) ((a) >= 'a' && (a) <= 'z')

static void trim_xilinx_version(char *x)
{
    for (;;) {
        x = strchr(x, '-');
        if (!x || strlen(x) < 7) {
            return;
        }
        if (DIGIT(x[1]) &&
                x[2] == '.' &&
                DIGIT(x[3]) &&
                DIGIT(x[4]) &&
                x[5] == '.' &&
                LOWER_CASE(x[6])) {
            *x = '\0';
            return;
        }
        x++;
    }
}

static void substitute_char(char *s, char a, char b)
{
    for (;;) {
        s = strchr(s, a);
        if (!s) {
            return;
        }
        *s = b;
        s++;
    }
}

static DeviceState *fdt_create_qdev_from_compat(char *compat, char **dev_type)
{
    DeviceState *ret = NULL;

    char *c = g_strdup(compat);
    ret = qdev_try_create(NULL, c);
    if (!ret) {
        /* QEMU substitutes "."s for ","s in device names, so try with that
         * substitutution
         */
        substitute_char(c, ',', '.');
        ret = qdev_try_create(NULL, c);
    }
    if (!ret) {
        /* try again with the xilinx version string trimmed */
        trim_xilinx_version(c);
        ret = qdev_try_create(NULL, c);
    }

    if (dev_type) {
        *dev_type = c;
    } else {
        g_free(c);
    }
    return ret;
}

static inline const char *trim_vendor(const char *s)
{
    /* FIXME: be more intelligent */
    const char *ret = memchr(s, ',', sizeof(s));
    return ret ? ret + 1 : s;
}

/*FIXME: roll into device tree functionality */

static inline uint64_t get_int_be(const void *p, int len)
{
    switch (len) {
    case 1:
        return *((uint8_t *)p);
    case 2:
        return be16_to_cpu(*((uint16_t *)p));
    case 4:
        return be32_to_cpu(*((uint32_t *)p));
    case 8:
        return be32_to_cpu(*((uint64_t *)p));
    default:
        fprintf(stderr, "unsupported integer length\n");
        abort();
    }
}

static int fdt_init_qdev(char *node_path, FDTMachineInfo *fdti, char *compat)
{
    int err;
    qemu_irq irq;
    target_phys_addr_t base;
    int offset;
    DeviceState *dev;
    char *dev_type = NULL;
    int is_intc;
    Error *errp = NULL;
    int i;

    dev = fdt_create_qdev_from_compat(compat, &dev_type);
    if (!dev) {
        DB_PRINT("no match found for %s\n", compat);
        return 1;
    }
    /* FIXME: attach to the sysbus instead */
    object_property_add_child(container_get(qdev_get_machine(), "/unattached"),
                              qemu_devtree_get_node_name(fdti->fdt, node_path),
                              OBJECT(dev), NULL);

    fdt_init_set_opaque(fdti, node_path, dev);

    /* connect nic if appropriate */
    static int nics;
    qdev_set_nic_properties(dev, &nd_table[nics]);
    if (nd_table[nics].instantiated) {
        DB_PRINT("NIC instantiated: %s\n", dev_type);
        nics++;
    }

    offset = fdt_path_offset(fdti->fdt, node_path);
    for (offset = fdt_first_property_offset(fdti->fdt, offset);
            offset != -FDT_ERR_NOTFOUND;
            offset = fdt_next_property_offset(fdti->fdt, offset)) {
        const char *propname;
        int len;
        const void *val = fdt_getprop_by_offset(fdti->fdt, offset,
                                                    &propname, &len);

        propname = trim_vendor(propname);
        ObjectProperty *p = object_property_find(OBJECT(dev), propname, NULL);
        if (p) {
            DB_PRINT("matched property: %s of type %s, len %d\n",
                                            propname, p->type, len);
        }
        if (!p) {
            continue;
        }

        /* FIXME: handle generically using accessors and stuff */
        if (!strcmp(p->type, "uint8") || !strcmp(p->type, "uint16") ||
                !strcmp(p->type, "uint32") || !strcmp(p->type, "uint64")) {
            object_property_set_int(OBJECT(dev), get_int_be(val, len), propname,
                                                                &errp);
            assert_no_error(errp);
            DB_PRINT("set property %s to %#llx\n", propname,
                                            (long long unsigned int)get_int_be(val, len));
        } else if (!strcmp(p->type, "bool")) {
            object_property_set_bool(OBJECT(dev), !!get_int_be(val, len),
                        propname, &errp);
            assert_no_error(errp);
            DB_PRINT("set property %s to %#llx\n", propname,
                                            (long long unsigned int)get_int_be(val, len));
        } else if (!strncmp(p->type, "link", 4)) {
            char target_node_path[DT_PATH_LENGTH];
            DeviceState *linked_dev;

            if (qemu_devtree_get_node_by_phandle(fdti->fdt, target_node_path,
                                                get_int_be(val, len))) {
                abort();
            }
            while (!fdt_init_has_opaque(fdti, target_node_path)) {
                fdt_init_yield(fdti);
            }
            linked_dev = fdt_init_get_opaque(fdti, target_node_path);
            object_property_set_link(OBJECT(dev), OBJECT(linked_dev), propname,
                                        &errp);
            assert_no_error(errp);
        }
    }

    qdev_init_nofail(dev);
    /* map slave attachment */
    base = qemu_devtree_getprop_cell(fdti->fdt, node_path, "reg", 0, false,
                                                                    &errp);
    assert_no_error(errp);
    sysbus_mmio_map(sysbus_from_qdev(dev), 0, base);

    {
        int len;
        fdt_get_property(fdti->fdt, fdt_path_offset(fdti->fdt, node_path),
                                "interrupt-controller", &len);
        is_intc = len >= 0;
        DB_PRINT("is interrupt controller: %c\n", is_intc ? 'y' : 'n');
    }
    /* connect irq */
    for (i = 0; ; ++i) {
        char irq_info[1024];
        irq = fdt_get_irq_info(fdti, node_path, i, &err, irq_info);
        /* INTCs inferr their top level, if no IRQ connection specified */
        if (err && is_intc) {
            irq = fdti->irq_base[0];
            sysbus_connect_irq(sysbus_from_qdev(dev), 0, irq);
            fprintf(stderr, "FDT: (%s) connected top level irq %s\n", dev_type,
                        irq_info);
            break;
        }
        if (!err) {
            sysbus_connect_irq(sysbus_from_qdev(dev), i, irq);
            fprintf(stderr, "FDT: (%s) connected irq %s\n", dev_type, irq_info);
        } else {
            break;
        }
    }

    if (dev_type) {
        g_free(dev_type);
    }

    return 0;
}

fdt_register_compatibility(simple_bus_fdt_init, "simple-bus");
