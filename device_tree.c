/*
 * Functions to help device tree manipulation using libfdt.
 * It also provides functions to read entries from device tree proc
 * interface.
 *
 * Copyright 2008 IBM Corporation.
 * Authors: Jerone Young <jyoung5@us.ibm.com>
 *          Hollis Blanchard <hollisb@us.ibm.com>
 *
 * This work is licensed under the GNU GPL license version 2 or later.
 *
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#include "qemu-common.h"
#include "qemu/error-report.h"
#include "sysemu/device_tree.h"
#include "sysemu/sysemu.h"
#include "hw/loader.h"
#include "hw/boards.h"
#include "qemu/config-file.h"

#include <libfdt.h>

#define FDT_MAX_SIZE  0x10000

void *create_device_tree(int *sizep)
{
    void *fdt;
    int ret;

    *sizep = FDT_MAX_SIZE;
    fdt = g_malloc0(FDT_MAX_SIZE);
    ret = fdt_create(fdt, FDT_MAX_SIZE);
    if (ret < 0) {
        goto fail;
    }
    ret = fdt_finish_reservemap(fdt);
    if (ret < 0) {
        goto fail;
    }
    ret = fdt_begin_node(fdt, "");
    if (ret < 0) {
        goto fail;
    }
    ret = fdt_end_node(fdt);
    if (ret < 0) {
        goto fail;
    }
    ret = fdt_finish(fdt);
    if (ret < 0) {
        goto fail;
    }
    ret = fdt_open_into(fdt, fdt, *sizep);
    if (ret) {
        error_report("Unable to copy device tree in memory");
        exit(1);
    }

    return fdt;
fail:
    error_report("%s Couldn't create dt: %s", __func__, fdt_strerror(ret));
    exit(1);
}

void *load_device_tree(const char *filename_path, int *sizep)
{
    int dt_size;
    int dt_file_load_size;
    int ret;
    void *fdt = NULL;

    *sizep = 0;
    dt_size = get_image_size(filename_path);
    if (dt_size < 0) {
        error_report("Unable to get size of device tree file '%s'",
                     filename_path);
        goto fail;
    }

    /* Expand to 2x size to give enough room for manipulation.  */
    dt_size += 10000;
    dt_size *= 2;
    /* First allocate space in qemu for device tree */
    fdt = g_malloc0(dt_size);

    dt_file_load_size = load_image(filename_path, fdt);
    if (dt_file_load_size < 0) {
        error_report("Unable to open device tree file '%s'",
                     filename_path);
        goto fail;
    }

    ret = fdt_open_into(fdt, fdt, dt_size);
    if (ret) {
        error_report("Unable to copy device tree in memory");
        goto fail;
    }

    /* Check sanity of device tree */
    if (fdt_check_header(fdt)) {
        error_report("Device tree file loaded into memory is invalid: %s",
                     filename_path);
        goto fail;
    }
    *sizep = dt_size;
    return fdt;

fail:
    g_free(fdt);
    return NULL;
}

static int findnode_nofail(void *fdt, const char *node_path)
{
    int offset;

    offset = fdt_path_offset(fdt, node_path);
    if (offset < 0) {
        error_report("%s Couldn't find node %s: %s", __func__, node_path,
                     fdt_strerror(offset));
        exit(1);
    }

    return offset;
}

int qemu_fdt_setprop(void *fdt, const char *node_path,
                     const char *property, const void *val, int size)
{
    int r;

    r = fdt_setprop(fdt, findnode_nofail(fdt, node_path), property, val, size);
    if (r < 0) {
        error_report("%s: Couldn't set %s/%s: %s", __func__, node_path,
                     property, fdt_strerror(r));
        exit(1);
    }

    return r;
}

int qemu_fdt_setprop_cell(void *fdt, const char *node_path,
                          const char *property, uint32_t val)
{
    int r;

    r = fdt_setprop_cell(fdt, findnode_nofail(fdt, node_path), property, val);
    if (r < 0) {
        error_report("%s: Couldn't set %s/%s = %#08x: %s", __func__,
                     node_path, property, val, fdt_strerror(r));
        exit(1);
    }

    return r;
}

int qemu_fdt_setprop_u64(void *fdt, const char *node_path,
                         const char *property, uint64_t val)
{
    val = cpu_to_be64(val);
    return qemu_fdt_setprop(fdt, node_path, property, &val, sizeof(val));
}

int qemu_fdt_setprop_string(void *fdt, const char *node_path,
                            const char *property, const char *string)
{
    int r;

    r = fdt_setprop_string(fdt, findnode_nofail(fdt, node_path), property, string);
    if (r < 0) {
        error_report("%s: Couldn't set %s/%s = %s: %s", __func__,
                     node_path, property, string, fdt_strerror(r));
        exit(1);
    }

    return r;
}

void *qemu_fdt_getprop(void *fdt, const char *node_path,
                       const char *property, int *lenp,
                       bool inherit, Error **errp)
{
    int len;
    const void *r;
    if (!lenp) {
        lenp = &len;
    }
    r = fdt_getprop(fdt, findnode_nofail(fdt, node_path), property, lenp);
    if (!r) {
        char parent[DT_PATH_LENGTH];
        if (inherit && !qemu_fdt_getparent(fdt, parent, node_path)) {
            return qemu_fdt_getprop(fdt, parent, property, lenp, true,
                                                                errp);
        }
        error_report("%s: Couldn't get %s/%s: %s\n", __func__,
                     node_path, property, fdt_strerror(*lenp));
        /* FIXME: Be smarter */
        error_set(errp, ERROR_CLASS_GENERIC_ERROR,
                  "Couldn't get %s/%s: %s", node_path, property, fdt_strerror(*lenp));
        return NULL;
    }
    return g_memdup(r, *lenp);
}

uint32_t qemu_fdt_getprop_cell(void *fdt, const char *node_path,
                               const char *property, int offset,
                               bool inherit, Error **errp)
{
    int len;
    uint32_t ret;
    const uint32_t *p = qemu_fdt_getprop(fdt, node_path, property, &len,
                                                                inherit, errp);
    if (errp && *errp) {
        return 0;
    }
    if (len < (offset+1)*4) {
        error_report("%s: %s/%s not long enough to hold %d properties "
                "(length = %d)\n", __func__, node_path, property,
                offset+1, len);
        /* FIXME: Be smarter */
        error_set(errp, ERROR_CLASS_GENERIC_ERROR,
                  "Not long enough to hold %d properties", offset+1);
        return 0;
    }
    ret = be32_to_cpu(p[offset]);
    g_free(p);
    return ret;
}

uint32_t qemu_fdt_get_phandle(void *fdt, const char *path)
{
    uint32_t r;

    r = fdt_get_phandle(fdt, findnode_nofail(fdt, path));
    if (r == 0) {
        error_report("%s: Couldn't get phandle for %s: %s", __func__,
                     path, fdt_strerror(r));
        exit(1);
    }

    return r;
}

int qemu_fdt_setprop_phandle(void *fdt, const char *node_path,
                             const char *property,
                             const char *target_node_path)
{
    uint32_t phandle = qemu_fdt_get_phandle(fdt, target_node_path);
    return qemu_fdt_setprop_cell(fdt, node_path, property, phandle);
}

uint32_t qemu_fdt_alloc_phandle(void *fdt)
{
    static int phandle = 0x0;

    /*
     * We need to find out if the user gave us special instruction at
     * which phandle id to start allocating phandles.
     */
    if (!phandle) {
        phandle = machine_phandle_start(current_machine);
    }

    if (!phandle) {
        /*
         * None or invalid phandle given on the command line, so fall back to
         * default starting point.
         */
        phandle = 0x8000;
    }

    return phandle++;
}

int qemu_fdt_nop_node(void *fdt, const char *node_path)
{
    int r;

    r = fdt_nop_node(fdt, findnode_nofail(fdt, node_path));
    if (r < 0) {
        error_report("%s: Couldn't nop node %s: %s", __func__, node_path,
                     fdt_strerror(r));
        exit(1);
    }

    return r;
}

int qemu_fdt_add_subnode(void *fdt, const char *name)
{
    char *dupname = g_strdup(name);
    char *basename = strrchr(dupname, '/');
    int retval;
    int parent = 0;

    if (!basename) {
        g_free(dupname);
        return -1;
    }

    basename[0] = '\0';
    basename++;

    if (dupname[0]) {
        parent = findnode_nofail(fdt, dupname);
    }

    retval = fdt_add_subnode(fdt, parent, basename);
    if (retval < 0) {
        error_report("FDT: Failed to create subnode %s: %s", name,
                     fdt_strerror(retval));
        exit(1);
    }

    g_free(dupname);
    return retval;
}

char *qemu_fdt_get_node_name(void *fdt, const char *node_path)
{
    const char *ret = fdt_get_name(fdt, fdt_path_offset(fdt, node_path), NULL);
    return ret ? strdup(ret) : NULL;
}

int qemu_fdt_get_node_depth(void *fdt, const char *node_path)
{
    return fdt_node_depth(fdt, fdt_path_offset(fdt, node_path));
}

static void qemu_fdt_children_info(void *fdt, const char *node_path,
        int depth, int *num, char **returned_paths) {
    int offset = fdt_path_offset(fdt, node_path);
    int root_depth = fdt_node_depth(fdt, offset);
    int cur_depth = root_depth;

    *num = 0;
    for (;;) {
        offset = fdt_next_node(fdt, offset, &cur_depth);
        if (cur_depth <= root_depth) {
            break;
        }
        if (cur_depth <= root_depth + depth || depth == 0) {
            if (returned_paths) {
                returned_paths[*num] = g_malloc0(DT_PATH_LENGTH);
                fdt_get_path(fdt, offset, returned_paths[*num], DT_PATH_LENGTH);
            }
            (*num)++;
        }
    }
}

char **qemu_fdt_get_children(void *fdt, const char *node_path, int depth)
{
    int num_children = qemu_fdt_get_num_children(fdt, node_path, depth);
    char **ret = g_malloc0(sizeof(*ret) * num_children);

    qemu_fdt_children_info(fdt, node_path, depth, &num_children, ret);
    return ret;
}

int qemu_fdt_get_num_children(void *fdt, const char *node_path, int depth)
{
    int ret;

    qemu_fdt_children_info(fdt, node_path, depth, &ret, NULL);
    return ret;
}

int qemu_fdt_node_by_compatible(void *fdt, char *node_path,
                        const char *compats)
{
    int offset = fdt_node_offset_by_compatible(fdt, 0, compats);
    return offset > 0 ?
        fdt_get_path(fdt, offset, node_path, DT_PATH_LENGTH) : 1;
}

int qemu_fdt_get_node_by_name(void *fdt, char *node_path,
        const char *cmpname) {
    int offset = 0;
    char *name = NULL;

    do {
        offset = fdt_next_node(fdt, offset, NULL);
        name = (void *)fdt_get_name(fdt, offset, NULL);
        if (!name) {
            continue;
        }
        if (!strncmp(name, cmpname, strlen(cmpname))) {
            break;
        }
    } while (offset > 0);
    return offset > 0 ?
        fdt_get_path(fdt, offset, node_path, DT_PATH_LENGTH) : 1;
}

int qemu_fdt_get_node_by_phandle(void *fdt, char *node_path, int phandle)
{
    return fdt_get_path(fdt, fdt_node_offset_by_phandle(fdt, phandle),
                            node_path, DT_PATH_LENGTH);
}

int qemu_fdt_getparent(void *fdt, char *node_path, const char *current)
{
    int offset = fdt_path_offset(fdt, current);
    int parent_offset = fdt_supernode_atdepth_offset(fdt, offset,
        fdt_node_depth(fdt, offset) - 1, NULL);

    return parent_offset > 0 ?
        fdt_get_path(fdt, parent_offset, node_path, DT_PATH_LENGTH) : 1;
}

int qemu_fdt_get_root_node(void *fdt, char *node_path)
{
    return fdt_get_path(fdt, 0, node_path, DT_PATH_LENGTH);
}

static void fdt_scan(void *fdt, int *num_nodes, int info_dump)
{
    int depth = 0, offset = 0;

    if (num_nodes) {
        *num_nodes = 0;
    }
    for (;;) {
        offset = fdt_next_node(fdt, offset, &depth);
        if (num_nodes) {
            (*num_nodes)++;
        }
        if (offset <= 0 || depth <= 0) {
            break;
        }

        if (info_dump) {
            char node_path[DT_PATH_LENGTH];
            char *all_compats = NULL;
            int compat_len;
            Error *errp = NULL;

            if (fdt_get_path(fdt, offset, node_path, DT_PATH_LENGTH)) {
                sprintf(node_path, "(none)");
            } else {
                all_compats = qemu_fdt_getprop(fdt, node_path, "compatible",
                                                    &compat_len, false, &errp);
            }
            if (!errp) {
                char *i = all_compats;
                for (;;) {
                    char *j = rawmemchr(i, '\0');
                    compat_len -= ((j+1)-i);
                    if (!compat_len) {
                        break;
                    }
                    *j = ' ';
                    i = j+1;
                }
            }
            printf("OFFSET: %d, DEPTH: %d, PATH: %s, COMPATS: %s\n",
                    offset, depth, node_path,
                    all_compats ? all_compats : "(none)");
        }
    }
}

void fdt_info_dump(void *fdt)
{
    fdt_scan(fdt, NULL, 1);
}

void qemu_fdt_dumpdtb(void *fdt, int size)
{
    const char *dumpdtb = qemu_opt_get(qemu_get_machine_opts(), "dumpdtb");

    if (dumpdtb) {
        /* Dump the dtb to a file and quit */
        exit(g_file_set_contents(dumpdtb, fdt, size, NULL) ? 0 : 1);
    }
}

int qemu_fdt_setprop_sized_cells_from_array(void *fdt,
                                            const char *node_path,
                                            const char *property,
                                            int numvalues,
                                            uint64_t *values)
{
    uint32_t *propcells;
    uint64_t value;
    int cellnum, vnum, ncells;
    uint32_t hival;
    int ret;

    propcells = g_new0(uint32_t, numvalues * 2);

    cellnum = 0;
    for (vnum = 0; vnum < numvalues; vnum++) {
        ncells = values[vnum * 2];
        if (ncells != 1 && ncells != 2) {
            ret = -1;
            goto out;
        }
        value = values[vnum * 2 + 1];
        hival = cpu_to_be32(value >> 32);
        if (ncells > 1) {
            propcells[cellnum++] = hival;
        } else if (hival != 0) {
            ret = -1;
            goto out;
        }
        propcells[cellnum++] = cpu_to_be32(value);
    }

    ret = qemu_fdt_setprop(fdt, node_path, property, propcells,
                           cellnum * sizeof(uint32_t));
out:
    g_free(propcells);
    return ret;
}

int fdt_get_num_nodes(void *fdt)
{
    int ret;

    fdt_scan(fdt, &ret, 0);
    return ret;
}
