/*
 * Altera Nios II helper routines.
 *
 * Copyright (C) 2012 Chris Wulff <crwulff@gmail.com>
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

#include "cpu.h"
#include "helper.h"

#if !defined(CONFIG_USER_ONLY)
#include "softmmu_exec.h"
#define MMUSUFFIX _mmu
#define SHIFT 0
#include "softmmu_template.h"
#define SHIFT 1
#include "softmmu_template.h"
#define SHIFT 2
#include "softmmu_template.h"
#define SHIFT 3
#include "softmmu_template.h"

void tlb_fill(CPUNios2State *env, target_ulong addr, int is_write, int mmu_idx,
              uintptr_t retaddr)
{
    TranslationBlock *tb;
    int ret;

    ret = cpu_nios2_handle_mmu_fault(env, addr, is_write, mmu_idx, 1);
    if (unlikely(ret)) {
        if (retaddr) {
            /* now we have a real cpu fault */
            tb = tb_find_pc(retaddr);
            if (tb) {
                /* The PC is inside the translated code. It means that we have
                   a virtual CPU fault */
                cpu_restore_state(tb, env, retaddr);
            }
        }
        cpu_loop_exit(env);
    }
}

void helper_raise_exception(CPUNios2State *env, uint32_t index)
{
    env->exception_index = index;
    cpu_loop_exit(env);
}

uint32_t helper_mmu_read(CPUNios2State *env, uint32_t rn)
{
    return mmu_read(env, rn);
}

void helper_mmu_write(CPUNios2State *env, uint32_t rn, uint32_t v)
{
    mmu_write(env, rn, v);
}

void helper_memalign(CPUNios2State *env, uint32_t addr, uint32_t dr, uint32_t wr, uint32_t mask)
{
    if (addr & mask) {
        qemu_log("unaligned access addr=%x mask=%x, wr=%d dr=r%d\n",
                 addr, mask, wr, dr);
        env->regs[CR_BADADDR] = addr;
        env->regs[CR_EXCEPTION] = EXCP_UNALIGN << 2;
        helper_raise_exception(env, EXCP_UNALIGN);
    }
}

void cpu_unassigned_access(CPUNios2State *env1, target_phys_addr_t addr,
                           int is_write, int is_exec, int is_asi, int size)
{
    qemu_log("unassigned access to %08X\n", addr);
}

uint32_t helper_divs(uint32_t a, uint32_t b)
{
    return (int32_t)a / (int32_t)b;
}

uint32_t helper_divu(uint32_t a, uint32_t b)
{
    return a / b;
}

#ifdef CALL_TRACING
void helper_call_status(uint32_t pc, uint32_t target)
{
    qemu_log("%08X: CALL %08X %s\n", pc, target, lookup_symbol(target));
}

void helper_eret_status(uint32_t pc)
{
    qemu_log("%08X: ERET STATUS %08X, ESTATUS %08X, EA %08X\n",
             pc, env->regs[CR_STATUS], env->regs[CR_ESTATUS], env->regs[R_EA]);
}

void helper_ret_status(uint32_t pc)
{
    qemu_log("%08X: RET RA %08X\n", pc, env->regs[R_RA]);
}
#endif

#endif /* !CONFIG_USER_ONLY */

