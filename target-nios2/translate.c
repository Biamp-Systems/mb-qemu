/*
 * Altera Nios II emulation for qemu: main translation routines.
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

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <assert.h>

#include "cpu.h"
#include "exec-all.h"
#include "disas.h"
#include "helper.h"
#include "qemu-common.h"

#include "instruction.h"

#define GEN_HELPER 1
#include "helper.h"

static const char *regnames[] = {
    "zero",     "at",       "r2",       "r3",
    "r4",       "r5",       "r6",       "r7",
    "r8",       "r9",       "r10",      "r11",
    "r12",      "r13",      "r14",      "r15",
    "r16",      "r17",      "r18",      "r19",
    "r20",      "r21",      "r22",      "r23",
    "et",       "bt",       "gp",       "sp",
    "fp",       "ea",       "ba",       "ra",
    "status",   "estatus",  "bstatus",  "ienable",
    "ipending", "cpuid",    "reserved", "exception",
    "pteaddr",  "tlbacc",   "tlbmisc",  "reserved",
    "badaddr",  "config",   "mpubase",  "mpuacc",
    "reserved", "reserved", "reserved", "reserved",
    "reserved", "reserved", "reserved", "reserved",
    "reserved", "reserved", "reserved", "reserved",
    "reserved", "reserved", "reserved", "reserved",
    "rpc"
};

static TCGv_ptr cpu_env;
static TCGv cpu_R[NUM_CORE_REGS];

#include "gen-icount.h"

/* generate intermediate code for basic block 'tb'.  */
static void gen_intermediate_code_internal(
    CPUNios2State *env, TranslationBlock *tb, int search_pc)
{
    DisasContext dc1, *dc = &dc1;
    int num_insns;
    int max_insns;
    uint32_t next_page_start;
    int j, lj = -1;
    uint16_t *gen_opc_end = gen_opc_buf + OPC_MAX_SIZE;

    /* Initialize DC */
    dc->env    = env;
    dc->cpu_R  = cpu_R;
    dc->is_jmp = DISAS_NEXT;
    dc->pc     = tb->pc;
    dc->tb     = tb;

    /* Dump the CPU state to the log */
    if (qemu_loglevel_mask(CPU_LOG_TB_IN_ASM)) {
        qemu_log("--------------\n");
        log_cpu_state(env, 0);
    }

    /* Set up instruction counts */
    num_insns = 0;
    max_insns = tb->cflags & CF_COUNT_MASK;
    if (max_insns == 0) {
        max_insns = CF_COUNT_MASK;
    }
    next_page_start = (tb->pc & TARGET_PAGE_MASK) + TARGET_PAGE_SIZE;

    gen_icount_start();
    do {
        /* Mark instruction start with associated PC */
        if (search_pc) {
            j = gen_opc_ptr - gen_opc_buf;
            if (lj < j) {
                lj++;
                while (lj < j) {
                    gen_opc_instr_start[lj++] = 0;
                }
            }
            gen_opc_pc[lj] = dc->pc;
            gen_opc_instr_start[lj] = 1;
            gen_opc_icount[lj] = num_insns;
        }

        LOG_DIS("%8.8x:\t", dc->pc);

        if (num_insns + 1 == max_insns && (tb->cflags & CF_LAST_IO)) {
            gen_io_start();
        }

        /* Decode an instruction */
        handle_instruction(dc);

        dc->pc += 4;
        num_insns++;

        /* Translation stops when a conditional branch is encountered.
         * Otherwise the subsequent code could get translated several times.
         * Also stop translation when a page boundary is reached.  This
         * ensures prefetch aborts occur at the right place.  */
    } while (!dc->is_jmp && gen_opc_ptr < gen_opc_end &&
             !env->singlestep_enabled &&
             !singlestep &&
             dc->pc < next_page_start &&
             num_insns < max_insns);

    if (tb->cflags & CF_LAST_IO) {
        gen_io_end();
    }

    /* Indicate where the next block should start */
    switch (dc->is_jmp) {
    case DISAS_NEXT:
        /* Save the current PC back into the CPU register */
        tcg_gen_movi_tl(cpu_R[R_PC], dc->pc);
        tcg_gen_exit_tb(0);
        break;

    default:
    case DISAS_JUMP:
    case DISAS_UPDATE:
        /* The jump will already have updated the PC register */
        tcg_gen_exit_tb(0);
        break;

    case DISAS_TB_JUMP:
        /* nothing more to generate */
        break;
    }

    /* End off the block */
    gen_icount_end(tb, num_insns);
    *gen_opc_ptr = INDEX_op_end;

    /* Mark instruction starts for the final generated instruction */
    if (search_pc) {
        j = gen_opc_ptr - gen_opc_buf;
        lj++;
        while (lj <= j) {
            gen_opc_instr_start[lj++] = 0;
        }
    } else {
        tb->size = dc->pc - tb->pc;
        tb->icount = num_insns;
    }

#ifdef DEBUG_DISAS
    if (qemu_loglevel_mask(CPU_LOG_TB_IN_ASM)) {
        qemu_log("----------------\n");
        qemu_log("IN: %s\n", lookup_symbol(tb->pc));
        log_target_disas(tb->pc, dc->pc - tb->pc, 0);
        qemu_log("\nisize=%d osize=%td\n",
                 dc->pc - tb->pc, gen_opc_ptr - gen_opc_buf);
    }
#endif
}

void gen_intermediate_code(CPUNios2State *env, TranslationBlock *tb)
{
    gen_intermediate_code_internal(env, tb, 0);
}

void gen_intermediate_code_pc(CPUNios2State *env, TranslationBlock *tb)
{
    gen_intermediate_code_internal(env, tb, 1);
}

void cpu_dump_state(CPUNios2State *env, FILE *f, fprintf_function cpu_fprintf,
                    int flags)
{
    int i;

    if (!env || !f) {
        return;
    }

    cpu_fprintf(f, "IN: PC=%x %s\n",
                env->regs[R_PC], lookup_symbol(env->regs[R_PC]));

    for (i = 0; i < NUM_CORE_REGS; i++) {
        cpu_fprintf(f, "%9s=%8.8x ", regnames[i], env->regs[i]);
        if ((i + 1) % 4 == 0) {
            cpu_fprintf(f, "\n");
        }
    }
    cpu_fprintf(f, " mmu write: VPN=%05X PID %02X TLBACC %08X\n",
                env->mmu.pteaddr_wr & CR_PTEADDR_VPN_MASK,
                (env->mmu.tlbmisc_wr & CR_TLBMISC_PID_MASK) >> 4,
                env->mmu.tlbacc_wr);
    cpu_fprintf(f, "\n\n");
}

Nios2CPU *cpu_nios2_init(const char *cpu_model)
{
    Nios2CPU *cpu;
    int i;

    cpu = NIOS2_CPU(object_new(TYPE_NIOS2_CPU));

    cpu->env.reset_addr = RESET_ADDRESS;
    cpu->env.exception_addr = EXCEPTION_ADDRESS;
    cpu->env.fast_tlb_miss_addr = FAST_TLB_MISS_ADDRESS;

    cpu_reset(CPU(cpu));
    qemu_init_vcpu(&cpu->env);

    cpu_env = tcg_global_reg_new_ptr(TCG_AREG0, "env");

    for (i = 0; i < NUM_CORE_REGS; i++) {
        cpu_R[i] = tcg_global_mem_new(TCG_AREG0,
                                      offsetof(CPUNios2State, regs[i]),
                                      regnames[i]);
    }

#define GEN_HELPER 2
#include "helper.h"

  return cpu;
}

void restore_state_to_opc(CPUNios2State *env, TranslationBlock *tb, int pc_pos)
{
    env->regs[R_PC] = gen_opc_pc[pc_pos];
}

