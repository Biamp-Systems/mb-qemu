/*
 * These files from binutils are concatenated:
 * nios2.h, nios2-isa.h, nios2-opc.c, nios2-dis.c
 */

/* nios2.h.  Altera New Jersey opcode list for GAS, the GNU assembler.

   Copyright (C) 2003
   by Nigel Gray (ngray@altera.com).

This file is part of GDB, GAS, and the GNU binutils.

GDB, GAS, and the GNU binutils are free software; you can redistribute
them and/or modify them under the terms of the GNU General Public
License as published by the Free Software Foundation; either version
1, or (at your option) any later version.

GDB, GAS, and the GNU binutils are distributed in the hope that they
will be useful, but WITHOUT ANY WARRANTY; without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this file; see the file COPYING.  If not,
see <http://www.gnu.org/licenses/>.  */

#ifndef _NIOS2_H_
#define _NIOS2_H_

#include "qemu/osdep.h"
#include "disas/bfd.h"

/****************************************************************************
 * This file contains structures, bit masks and shift counts used
 * by the GNU toolchain to define the New Jersey instruction set and
 * access various opcode fields.
 ****************************************************************************/

enum overflow_type {
    call_target_overflow = 0,
    branch_target_overflow,
    address_offset_overflow,
    signed_immed16_overflow,
    unsigned_immed16_overflow,
    unsigned_immed5_overflow,
    custom_opcode_overflow,
    no_overflow
};

/*---------------------------------------------------------------------------
   This structure holds information for a particular instruction
  ---------------------------------------------------------------------------*/

/* match When assembling, this
     opcode is modified by the arguments to produce the actual opcode
     that is used.  If pinfo is INSN_MACRO, then this is 0.  */

/* mask If pinfo is not INSN_MACRO, then this is a bit mask for the
     relevant portions of the opcode when disassembling.  If the
     actual opcode anded with the match field equals the opcode field,
     then we have found the correct instruction.  If pinfo is
     INSN_MACRO, then this field is the macro identifier.  */

/* For a macro, this is INSN_MACRO.  Otherwise, it is a collection
     of bits describing the instruction, notably any relevant hazard
     information.  */

struct nios2_opcode {
    const char *name;                /* The name of the instruction.  */
    const char *args;                /* A string describing the arguments
                                        for this instruction.  */
    const char *args_test;           /* Like args, but with an extra argument
                                        for the expected opcode */
    unsigned long num_args;          /* the number of arguments the instruction
                                        takes */
    unsigned long match;             /* The basic opcode for the instruction. */
    unsigned long mask;              /* mask for the opcode field of the
                                        instruction */
    unsigned long pinfo;             /* is this a real instruction or
                                        instruction macro */
    enum overflow_type overflow_msg; /* msg template used to generate
                                        informative message when fixup
                                        overflows */
};

/* This value is used in the nios2_opcode.pinfo field to indicate that the
   instruction is a macro or pseudo-op. This requires special treatment by
   the assembler, and is used by the disassembler to determine whether to
   check for a nop */
#define NIOS2_INSN_MACRO        0x80000000
#define NIOS2_INSN_MACRO_MOV    0x80000001
#define NIOS2_INSN_MACRO_MOVI   0x80000002
#define NIOS2_INSN_MACRO_MOVIA  0x80000004

#define NIOS2_INSN_RELAXABLE    0x40000000
#define NIOS2_INSN_UBRANCH      0x00000010
#define NIOS2_INSN_CBRANCH      0x00000020
#define NIOS2_INSN_CALL         0x00000040

#define NIOS2_INSN_ADDI         0x00000080
#define NIOS2_INSN_ANDI         0x00000100
#define NIOS2_INSN_ORI          0x00000200
#define NIOS2_INSN_XORI         0x00000400



/* Associates a register name ($6) with a 5-bit index (eg 6) */
struct nios2_reg {
    const char *name;
    const int index;
};


/* -------------------------------------------------------------------------
    Bitfield masks for New Jersey instructions
   -------------------------------------------------------------------------*/

/* These are bit masks and shift counts to use to access the various
   fields of an instruction. */

/* Macros for getting and setting an instruction field */
#define GET_INSN_FIELD(X, i)     (((i) & OP_MASK_##X) >> OP_SH_##X)
#define SET_INSN_FIELD(X, i, j)  ((i) = ((i) & ~(OP_MASK_##X)) | \
                                        ((j) << OP_SH_##X))


/*
   We include the auto-generated file nios2-isa.h and define the mask
   and shifts below in terms of those in nios2-isa.h. This ensures
   that the binutils and hardware are always in sync
*/

/*#include "nios2-isa.h"*/

#define OP_MASK_OP              (IW_OP_MASK << IW_OP_LSB)
#define OP_SH_OP                IW_OP_LSB


/* Masks and shifts for I-type instructions */

#define OP_MASK_IOP             (IW_OP_MASK << IW_OP_LSB)
#define OP_SH_IOP               IW_OP_LSB

#define OP_MASK_IMM16           (IW_IMM16_MASK << IW_IMM16_LSB)
#define OP_SH_IMM16             IW_IMM16_LSB

#define OP_MASK_IRD             (IW_B_MASK << IW_B_LSB)
#define OP_SH_IRD               IW_B_LSB

#define OP_MASK_IRT             (IW_B_MASK << IW_B_LSB)
#define OP_SH_IRT               IW_B_LSB

#define OP_MASK_IRS             (IW_A_MASK << IW_A_LSB)
#define OP_SH_IRS               IW_A_LSB

/* Masks and shifts for R-type instructions */

#define OP_MASK_ROP             (IW_OP_MASK << IW_OP_LSB)
#define OP_SH_ROP               IW_OP_LSB

#define OP_MASK_ROPX            (IW_OPX_MASK << IW_OPX_LSB)
#define OP_SH_ROPX              IW_OPX_LSB

#define OP_MASK_RRD             (IW_C_MASK << IW_C_LSB)
#define OP_SH_RRD               IW_C_LSB

#define OP_MASK_RRT             (IW_B_MASK << IW_B_LSB)
#define OP_SH_RRT               IW_B_LSB

#define OP_MASK_RRS             (IW_A_MASK << IW_A_LSB)
#define OP_SH_RRS               IW_A_LSB

/* Masks and shifts for J-type instructions */

#define OP_MASK_JOP             (IW_OP_MASK << IW_OP_LSB)
#define OP_SH_JOP               IW_OP_LSB

#define OP_MASK_IMM26           (IW_IMM26_MASK << IW_IMM26_LSB)
#define OP_SH_IMM26             IW_IMM26_LSB

/* Masks and shifts for CTL instructions */

#define OP_MASK_RCTL    0x000007c0
#define OP_SH_RCTL              6

/* break instruction imm5 field */
#define OP_MASK_TRAP_IMM5 0x000007c0
#define OP_SH_TRAP_IMM5   6

/* instruction imm5 field */
#define OP_MASK_IMM5            (IW_SHIFT_IMM5_MASK << IW_SHIFT_IMM5_LSB)
#define OP_SH_IMM5              IW_SHIFT_IMM5_LSB

/* cache operation fields (type j,i(s)) */
#define OP_MASK_CACHE_OPX       (IW_B_MASK << IW_B_LSB)
#define OP_SH_CACHE_OPX         IW_B_LSB
#define OP_MASK_CACHE_RRS       (IW_A_MASK << IW_A_LSB)
#define OP_SH_CACHE_RRS         IW_A_LSB

/* custom instruction masks */
#define OP_MASK_CUSTOM_A                0x00010000
#define OP_SH_CUSTOM_A                          16

#define OP_MASK_CUSTOM_B                0x00008000
#define OP_SH_CUSTOM_B                          15

#define OP_MASK_CUSTOM_C                0x00004000
#define OP_SH_CUSTOM_C                          14

#define OP_MASK_CUSTOM_N                0x00003fc0
#define OP_SH_CUSTOM_N                           6
#define OP_MAX_CUSTOM_N                        255

/*
       The following macros define the opcode matches for each
       instruction
       code & OP_MASK_INST == OP_MATCH_INST
 */

/* OP instruction matches */
#define OP_MATCH_ADDI           OP_ADDI
#define OP_MATCH_ANDHI          OP_ANDHI
#define OP_MATCH_ANDI           OP_ANDI
#define OP_MATCH_BEQ            OP_BEQ
#define OP_MATCH_BGE            OP_BGE
#define OP_MATCH_BGEU           OP_BGEU
#define OP_MATCH_BLT            OP_BLT
#define OP_MATCH_BLTU           OP_BLTU
#define OP_MATCH_BNE            OP_BNE
#define OP_MATCH_BR             OP_BR
#define OP_MATCH_FLUSHD         OP_FLUSHD
#define OP_MATCH_FLUSHDA        OP_FLUSHDA
#define OP_MATCH_INITD          OP_INITD
#define OP_MATCH_INITDA         OP_INITDA
#define OP_MATCH_CALL           OP_CALL
#define OP_MATCH_CMPEQI         OP_CMPEQI
#define OP_MATCH_CMPGEI         OP_CMPGEI
#define OP_MATCH_CMPGEUI        OP_CMPGEUI
#define OP_MATCH_CMPLTI         OP_CMPLTI
#define OP_MATCH_CMPLTUI        OP_CMPLTUI
#define OP_MATCH_CMPNEI         OP_CMPNEI
#define OP_MATCH_JMPI           OP_JMPI
#define OP_MATCH_LDB            OP_LDB
#define OP_MATCH_LDBIO          OP_LDBIO
#define OP_MATCH_LDBU           OP_LDBU
#define OP_MATCH_LDBUIO         OP_LDBUIO
#define OP_MATCH_LDH            OP_LDH
#define OP_MATCH_LDHIO          OP_LDHIO
#define OP_MATCH_LDHU           OP_LDHU
#define OP_MATCH_LDHUIO         OP_LDHUIO
#define OP_MATCH_LDL            OP_LDL
#define OP_MATCH_LDW            OP_LDW
#define OP_MATCH_LDWIO          OP_LDWIO
#define OP_MATCH_MULI           OP_MULI
#define OP_MATCH_OPX            OP_OPX
#define OP_MATCH_ORHI           OP_ORHI
#define OP_MATCH_ORI            OP_ORI
#define OP_MATCH_STB            OP_STB
#define OP_MATCH_STBIO          OP_STBIO
#define OP_MATCH_STC            OP_STC
#define OP_MATCH_STH            OP_STH
#define OP_MATCH_STHIO          OP_STHIO
#define OP_MATCH_STW            OP_STW
#define OP_MATCH_STWIO          OP_STWIO
#define OP_MATCH_CUSTOM         OP_CUSTOM
#define OP_MATCH_XORHI          OP_XORHI
#define OP_MATCH_XORI           OP_XORI
#define OP_MATCH_OPX            OP_OPX



/* OPX instruction values */
#define OP_MATCH_ADD    ((OPX_ADD << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_AND    ((OPX_AND << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_BREAK  ((0x1e << 17) | (OPX_BREAK << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_BRET   ((0xf0000000) | (OPX_BRET << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_CALLR  ((0x1f << 17) | (OPX_CALLR << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_CMPEQ  ((OPX_CMPEQ << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_CMPGE  ((OPX_CMPGE << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_CMPGEU ((OPX_CMPGEU << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_CMPLT  ((OPX_CMPLT << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_CMPLTU ((OPX_CMPLTU << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_CMPNE  ((OPX_CMPNE << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_DIV    ((OPX_DIV << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_DIVU   ((OPX_DIVU << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_JMP    ((OPX_JMP << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_MUL    ((OPX_MUL << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_MULXSS ((OPX_MULXSS << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_MULXSU ((OPX_MULXSU << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_MULXUU ((OPX_MULXUU << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_NEXTPC ((OPX_NEXTPC << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_NOR    ((OPX_NOR << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_OR     ((OPX_OR << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_RDCTL  ((OPX_RDCTL << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_RET    ((0xf8000000) | (OPX_RET << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_ROL    ((OPX_ROL << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_ROLI   ((OPX_ROLI << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_ROR    ((OPX_ROR << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_SLL    ((OPX_SLL << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_SLLI   ((OPX_SLLI << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_SRA    ((OPX_SRA << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_SRAI   ((OPX_SRAI << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_SRL    ((OPX_SRL << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_SRLI   ((OPX_SRLI << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_SUB    ((OPX_SUB << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_SYNC   ((OPX_SYNC << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_TRAP   ((0x1d << 17) | (OPX_TRAP << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_ERET   ((0xe8000000) | (OPX_ERET << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_WRCTL  ((OPX_WRCTL << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_XOR    ((OPX_XOR << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_FLUSHI ((OPX_FLUSHI << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_FLUSHP ((OPX_FLUSHP << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_INITI  ((OPX_INITI << IW_OPX_LSB) | (OP_OPX))

/*
       Some unusual op masks
*/
#define OP_MASK_BREAK  ((OP_MASK_RRS | OP_MASK_RRT | OP_MASK_RRD | \
                         OP_MASK_ROPX | OP_MASK_OP) & 0xfffff03f)
#define OP_MASK_CALLR  ((OP_MASK_RRT | OP_MASK_RRD | OP_MASK_ROPX | OP_MASK_OP))
#define OP_MASK_JMP    ((OP_MASK_RRT | OP_MASK_RRD | OP_MASK_ROPX | OP_MASK_OP))
#define OP_MASK_SYNC   ((OP_MASK_RRT | OP_MASK_RRD | OP_MASK_ROPX | OP_MASK_OP))
#define OP_MASK_TRAP   ((OP_MASK_RRS | OP_MASK_RRT | OP_MASK_RRD | \
                         OP_MASK_ROPX | OP_MASK_OP) & 0xfffff83f)
#define OP_MASK_WRCTL  ((OP_MASK_RRT | OP_MASK_RRD | OP_MASK_ROPX | OP_MASK_OP))
#define OP_MASK_NEXTPC ((OP_MASK_RRS | OP_MASK_RRT | OP_MASK_ROPX | OP_MASK_OP))
#define OP_MASK_FLUSHI ((OP_MASK_RRT | OP_MASK_RRD | OP_MASK_ROPX | OP_MASK_OP))
#define OP_MASK_INITI  ((OP_MASK_RRT | OP_MASK_RRD | OP_MASK_ROPX | OP_MASK_OP))

#define OP_MASK_ROLI   ((OP_MASK_RRT | OP_MASK_ROPX | OP_MASK_OP))
#define OP_MASK_SLLI   ((OP_MASK_RRT | OP_MASK_ROPX | OP_MASK_OP))
#define OP_MASK_SRAI   ((OP_MASK_RRT | OP_MASK_ROPX | OP_MASK_OP))
#define OP_MASK_SRLI   ((OP_MASK_RRT | OP_MASK_ROPX | OP_MASK_OP))
#define OP_MASK_RDCTL  ((OP_MASK_RRS | OP_MASK_RRT | OP_MASK_ROPX | OP_MASK_OP))

#ifndef OP_MASK
#define OP_MASK                         0xffffffff
#endif

/* These are the data structures we use to hold the instruction information */

extern const struct nios2_opcode nios2_builtin_opcodes[];
extern const int bfd_nios2_num_builtin_opcodes;
extern struct nios2_opcode *nios2_opcodes;
extern int bfd_nios2_num_opcodes;

/* These are the data structures used to hold the register information */
extern const struct nios2_reg nios2_builtin_regs[];
extern struct nios2_reg *nios2_regs;
extern const int nios2_num_builtin_regs;
extern int nios2_num_regs;

/* Machine-independent macro for number of opcodes */

#define NUMOPCODES bfd_nios2_num_opcodes
#define NUMREGISTERS nios2_num_regs;

/* these are used in disassembly to get the correct register names */
#define NUMREGNAMES 32
#define NUMCTLREGNAMES 32
#define CTLREGBASE     42
#define COPROCREGBASE  83
#define NUMCOPROCREGNAMES 32


/* this is made extern so that the assembler can use it to find out
   what instruction caused an error */
extern const struct nios2_opcode *nios2_find_opcode_hash(unsigned long);

/* overflow message strings used in the assembler */
extern const char *overflow_msgs[];

#endif /* _NIOS2_H */
/*
 * This file defines Nios II instruction set constants.
 * To include it in assembly code (.S file), define ALT_ASM_SRC
 * before including this file.
 *
 * This file is automatically generated by gen_isa.pl - do not edit
 */

#ifndef _NIOS2_ISA_H_
#define _NIOS2_ISA_H_

/* OP instruction opcode values (index is OP field) */
#define NUM_OP_INSTS 64

#ifndef ALT_ASM_SRC
extern const char *op_names[NUM_OP_INSTS];
#endif /* ALT_ASM_SRC */

/* OPX instruction opcode values (index is OPX field) */
#define NUM_OPX_INSTS 64

#ifndef ALT_ASM_SRC
extern const char *opx_names[NUM_OPX_INSTS];
#endif /* ALT_ASM_SRC */

/* Constants for instruction fields and ISA */
#define BREAK_INST_EXC_ID 7
#define CPU_RESET_CAUSE_ID 1
#define CPU_RESET_EXC_ID 1
#define DIV_ERROR_CAUSE_ID 8
#define DIV_ERROR_EXC_ID 10
#define EMPTY_SLAVE_DATA_ACCESS_ERROR_CAUSE_ID 30
#define EMPTY_SLAVE_DATA_ACCESS_ERROR_EXC_ID 22
#define EMPTY_SLAVE_INST_ACCESS_ERROR_CAUSE_ID 29
#define EMPTY_SLAVE_INST_ACCESS_ERROR_EXC_ID 21
#define HBREAK_EXC_ID 2
#define ILLEGAL_INST_CAUSE_ID 5
#define ILLEGAL_INST_EXC_ID 6
#define MAX_CAUSE_ID 31
#define MISALIGNED_DATA_ADDR_CAUSE_ID 6
#define MISALIGNED_DATA_ADDR_EXC_ID 8
#define MISALIGNED_TARGET_PC_CAUSE_ID 7
#define MISALIGNED_TARGET_PC_EXC_ID 9
#define MPU_DATA_REGION_VIOLATION_CAUSE_ID 17
#define MPU_DATA_REGION_VIOLATION_EXC_ID 20
#define MPU_INST_REGION_VIOLATION_CAUSE_ID 16
#define MPU_INST_REGION_VIOLATION_EXC_ID 19
#define NIOS2_DISPLAY_INST_TRACE 1
#define NIOS2_DISPLAY_MEM_TRAFFIC 2
#define NONE_EXC_ID 0
#define NORM_INTR_CAUSE_ID 2
#define NORM_INTR_EXC_ID 3
#define NUM_EXC_IDS 24
#define READONLY_SLAVE_DATA_ACCESS_ERROR_CAUSE_ID 31
#define READONLY_SLAVE_DATA_ACCESS_ERROR_EXC_ID 23
#define RECORD_DATA_ADDR 2
#define RECORD_NOTHING 0
#define RECORD_TARGET_PCB 1
#define RESET_CAUSE_ID 0
#define SUPERVISOR_DATA_ADDR_CAUSE_ID 11
#define SUPERVISOR_DATA_ADDR_EXC_ID 13
#define SUPERVISOR_INST_ADDR_CAUSE_ID 9
#define SUPERVISOR_INST_ADDR_EXC_ID 11
#define SUPERVISOR_INST_CAUSE_ID 10
#define SUPERVISOR_INST_EXC_ID 12
#define TLB_DATA_MISS_EXC_ID 15
#define TLB_INST_MISS_EXC_ID 14
#define TLB_MISS_CAUSE_ID 12
#define TLB_R_PERM_CAUSE_ID 14
#define TLB_R_PERM_EXC_ID 17
#define TLB_W_PERM_CAUSE_ID 15
#define TLB_W_PERM_EXC_ID 18
#define TLB_X_PERM_CAUSE_ID 13
#define TLB_X_PERM_EXC_ID 16
#define TRAP_INST_CAUSE_ID 3
#define TRAP_INST_EXC_ID 4
#define UNIMP_INST_CAUSE_ID 4
#define UNIMP_INST_EXC_ID 5
#define AT_REGNUM 1
#define BADADDR_REG_BADDR_LSB 0
#define BADADDR_REG_BADDR_MSB 31
#define BADADDR_REG_BADDR_SZ 32
#define BADADDR_REG_BADDR_MASK 0xffffffff
#define BADADDR_REG_LSB 0
#define BADADDR_REG_MSB 31
#define BADADDR_REG_REGNUM 12
#define BADADDR_REG_SZ 32
#define BADADDR_REG_MASK 0xffffffff
#define BRETADDR_REGNUM 30
#define BSTATUS_REG_LSB 0
#define BSTATUS_REG_MMU_LSB 0
#define BSTATUS_REG_MMU_MSB 2
#define BSTATUS_REG_MMU_SZ 3
#define BSTATUS_REG_MMU_MASK 0x7
#define BSTATUS_REG_MPU_LSB 0
#define BSTATUS_REG_MPU_MSB 1
#define BSTATUS_REG_MPU_SZ 2
#define BSTATUS_REG_MPU_MASK 0x3
#define BSTATUS_REG_MSB 2
#define BSTATUS_REG_NO_MMU_LSB 0
#define BSTATUS_REG_NO_MMU_MSB 0
#define BSTATUS_REG_NO_MMU_SZ 1
#define BSTATUS_REG_NO_MMU_MASK 0x1
#define BSTATUS_REG_REGNUM 2
#define BSTATUS_REG_SZ 3
#define BSTATUS_REG_MASK 0x7
#define BT_REGNUM 25
#define CACHE_MAX_BYTES 65536
#define CACHE_MAX_LINE_BYTES 32
#define CACHE_MIN_LINE_BYTES 4
#define COMPARE_OP_EQ 0x0
#define COMPARE_OP_GE 0x1
#define COMPARE_OP_LSB 3
#define COMPARE_OP_LT 0x2
#define COMPARE_OP_MSB 4
#define COMPARE_OP_NE 0x3
#define COMPARE_OP_SZ 2
#define COMPARE_OP_MASK 0x3
#define CONFIG_REG_LSB 0
#define CONFIG_REG_MSB 0
#define CONFIG_REG_PE_LSB 0
#define CONFIG_REG_PE_MSB 0
#define CONFIG_REG_PE_SZ 1
#define CONFIG_REG_PE_MASK 0x1
#define CONFIG_REG_REGNUM 13
#define CONFIG_REG_SZ 1
#define CONFIG_REG_MASK 0x1
#define CPUID_REG_LSB 0
#define CPUID_REG_MSB 31
#define CPUID_REG_REGNUM 5
#define CPUID_REG_SZ 32
#define CPUID_REG_MASK 0xffffffff
#define DATAPATH_LOG2_SZ 5
#define DATAPATH_LOG2_MASK 0x1f
#define DATAPATH_LSB 0
#define DATAPATH_MSB 31
#define DATAPATH_SZ 32
#define DATAPATH_MASK 0xffffffff
#define EMPTY_CRST_IW 127034
#define EMPTY_HBREAK_IW 4040762
#define EMPTY_INTR_IW 3926074
#define EMPTY_NOP_IW 100410
#define EMPTY_RET_IW 4160759866
#define ERETADDR_REGNUM 29
#define ESTATUS_REG_LSB 0
#define ESTATUS_REG_MMU_LSB 0
#define ESTATUS_REG_MMU_MSB 2
#define ESTATUS_REG_MMU_SZ 3
#define ESTATUS_REG_MMU_MASK 0x7
#define ESTATUS_REG_MPU_LSB 0
#define ESTATUS_REG_MPU_MSB 1
#define ESTATUS_REG_MPU_SZ 2
#define ESTATUS_REG_MPU_MASK 0x3
#define ESTATUS_REG_MSB 2
#define ESTATUS_REG_NO_MMU_LSB 0
#define ESTATUS_REG_NO_MMU_MSB 0
#define ESTATUS_REG_NO_MMU_SZ 1
#define ESTATUS_REG_NO_MMU_MASK 0x1
#define ESTATUS_REG_REGNUM 1
#define ESTATUS_REG_SZ 3
#define ESTATUS_REG_MASK 0x7
#define ET_REGNUM 24
#define EXCEPTION_REG_CAUSE_LSB 2
#define EXCEPTION_REG_CAUSE_MSB 6
#define EXCEPTION_REG_CAUSE_SZ 5
#define EXCEPTION_REG_CAUSE_MASK 0x1f
#define EXCEPTION_REG_LSB 0
#define EXCEPTION_REG_MEA_LSB 0
#define EXCEPTION_REG_MEA_MSB 0
#define EXCEPTION_REG_MEA_SZ 1
#define EXCEPTION_REG_MEA_MASK 0x1
#define EXCEPTION_REG_MEE_LSB 1
#define EXCEPTION_REG_MEE_MSB 1
#define EXCEPTION_REG_MEE_SZ 1
#define EXCEPTION_REG_MEE_MASK 0x1
#define EXCEPTION_REG_MSB 6
#define EXCEPTION_REG_REGNUM 7
#define EXCEPTION_REG_SZ 7
#define EXCEPTION_REG_MASK 0x7f
#define FP_REGNUM 28
#define FSTATUS_REG_REGNUM 11
#define GP_REGNUM 26
#define IENABLE_REG_LSB 0
#define IENABLE_REG_MSB 31
#define IENABLE_REG_REGNUM 3
#define IENABLE_REG_SZ 32
#define IENABLE_REG_MASK 0xffffffff
#define IPENDING_REG_LSB 0
#define IPENDING_REG_MSB 31
#define IPENDING_REG_REGNUM 4
#define IPENDING_REG_SZ 32
#define IPENDING_REG_MASK 0xffffffff
#define IW_A_LSB 27
#define IW_A_MSB 31
#define IW_A_SZ 5
#define IW_A_MASK 0x1f
#define IW_B_LSB 22
#define IW_B_MSB 26
#define IW_B_SZ 5
#define IW_B_MASK 0x1f
#define IW_C_LSB 17
#define IW_C_MSB 21
#define IW_C_SZ 5
#define IW_C_MASK 0x1f
#define IW_CONTROL_REGNUM_BASE 0
#define IW_CONTROL_REGNUM_LSB 6
#define IW_CONTROL_REGNUM_MSB 9
#define IW_CONTROL_REGNUM_SZ 4
#define IW_CONTROL_REGNUM_MASK 0xf
#define IW_CUSTOM_N_LSB 6
#define IW_CUSTOM_N_MSB 13
#define IW_CUSTOM_N_SZ 8
#define IW_CUSTOM_N_MASK 0xff
#define IW_CUSTOM_READRA_LSB 16
#define IW_CUSTOM_READRA_MSB 16
#define IW_CUSTOM_READRA_SZ 1
#define IW_CUSTOM_READRA_MASK 0x1
#define IW_CUSTOM_READRB_LSB 15
#define IW_CUSTOM_READRB_MSB 15
#define IW_CUSTOM_READRB_SZ 1
#define IW_CUSTOM_READRB_MASK 0x1
#define IW_CUSTOM_WRITERC_LSB 14
#define IW_CUSTOM_WRITERC_MSB 14
#define IW_CUSTOM_WRITERC_SZ 1
#define IW_CUSTOM_WRITERC_MASK 0x1
#define IW_IMM16_LSB 6
#define IW_IMM16_MSB 21
#define IW_IMM16_SZ 16
#define IW_IMM16_MASK 0xffff
#define IW_IMM26_LSB 6
#define IW_IMM26_MSB 31
#define IW_IMM26_SZ 26
#define IW_IMM26_MASK 0x3ffffff
#define IW_MEMSZ_BYTE 0x0
#define IW_MEMSZ_HWORD 0x1
#define IW_MEMSZ_LSB 3
#define IW_MEMSZ_MSB 4
#define IW_MEMSZ_SZ 2
#define IW_MEMSZ_MASK 0x3
#define IW_MEMSZ_WORD_MSB 0x1
#define IW_OP_LSB 0
#define IW_OP_MSB 5
#define IW_OP_SZ 6
#define IW_OP_MASK 0x3f
#define IW_OPX_LSB 11
#define IW_OPX_MSB 16
#define IW_OPX_SZ 6
#define IW_OPX_MASK 0x3f
#define IW_SHIFT_IMM5_LSB 6
#define IW_SHIFT_IMM5_MSB 10
#define IW_SHIFT_IMM5_SZ 5
#define IW_SHIFT_IMM5_MASK 0x1f
#define IW_SZ 32
#define IW_MASK 0xffffffff
#define IW_TRAP_BREAK_IMM5_LSB 6
#define IW_TRAP_BREAK_IMM5_MSB 10
#define IW_TRAP_BREAK_IMM5_SZ 5
#define IW_TRAP_BREAK_IMM5_MASK 0x1f
#define JMP_CALLR_VS_RET_IS_RET 0
#define JMP_CALLR_VS_RET_OPX_BIT 3
#define LOGIC_OP_AND 0x1
#define LOGIC_OP_LSB 3
#define LOGIC_OP_MSB 4
#define LOGIC_OP_NOR 0x0
#define LOGIC_OP_OR 0x2
#define LOGIC_OP_SZ 2
#define LOGIC_OP_MASK 0x3
#define LOGIC_OP_XOR 0x3
#define MMU_ADDR_BYPASS_TLB 0x3
#define MMU_ADDR_BYPASS_TLB_CACHEABLE 0x0
#define MMU_ADDR_BYPASS_TLB_CACHEABLE_LSB 29
#define MMU_ADDR_BYPASS_TLB_CACHEABLE_MSB 29
#define MMU_ADDR_BYPASS_TLB_CACHEABLE_SZ 1
#define MMU_ADDR_BYPASS_TLB_CACHEABLE_MASK 0x1
#define MMU_ADDR_BYPASS_TLB_LSB 30
#define MMU_ADDR_BYPASS_TLB_MSB 31
#define MMU_ADDR_BYPASS_TLB_PADDR_LSB 0
#define MMU_ADDR_BYPASS_TLB_PADDR_MSB 28
#define MMU_ADDR_BYPASS_TLB_PADDR_SZ 29
#define MMU_ADDR_BYPASS_TLB_PADDR_MASK 0x1fffffff
#define MMU_ADDR_BYPASS_TLB_SZ 2
#define MMU_ADDR_BYPASS_TLB_MASK 0x3
#define MMU_ADDR_IO_REGION 0x7
#define MMU_ADDR_IO_REGION_LSB 29
#define MMU_ADDR_IO_REGION_MSB 31
#define MMU_ADDR_IO_REGION_SZ 3
#define MMU_ADDR_IO_REGION_MASK 0x7
#define MMU_ADDR_IO_REGION_VPN 0xe0000
#define MMU_ADDR_KERNEL_MMU_REGION 0x2
#define MMU_ADDR_KERNEL_MMU_REGION_LSB 30
#define MMU_ADDR_KERNEL_MMU_REGION_MSB 31
#define MMU_ADDR_KERNEL_MMU_REGION_SZ 2
#define MMU_ADDR_KERNEL_MMU_REGION_MASK 0x3
#define MMU_ADDR_KERNEL_REGION 0x6
#define MMU_ADDR_KERNEL_REGION_INT 6
#define MMU_ADDR_KERNEL_REGION_LSB 29
#define MMU_ADDR_KERNEL_REGION_MSB 31
#define MMU_ADDR_KERNEL_REGION_SZ 3
#define MMU_ADDR_KERNEL_REGION_MASK 0x7
#define MMU_ADDR_PAGE_OFFSET_LSB 0
#define MMU_ADDR_PAGE_OFFSET_MSB 11
#define MMU_ADDR_PAGE_OFFSET_SZ 12
#define MMU_ADDR_PAGE_OFFSET_MASK 0xfff
#define MMU_ADDR_PFN_LSB 12
#define MMU_ADDR_PFN_MSB 31
#define MMU_ADDR_PFN_SZ 20
#define MMU_ADDR_PFN_MASK 0xfffff
#define MMU_ADDR_USER_REGION 0x0
#define MMU_ADDR_USER_REGION_LSB 31
#define MMU_ADDR_USER_REGION_MSB 31
#define MMU_ADDR_USER_REGION_SZ 1
#define MMU_ADDR_USER_REGION_MASK 0x1
#define MMU_ADDR_VPN_LSB 12
#define MMU_ADDR_VPN_MSB 31
#define MMU_ADDR_VPN_SZ 20
#define MMU_ADDR_VPN_MASK 0xfffff
#define MPU_DATA_PERM_SUPER_NONE_USER_NONE 0
#define MPU_DATA_PERM_SUPER_RD_USER_NONE 1
#define MPU_DATA_PERM_SUPER_RD_USER_RD 2
#define MPU_DATA_PERM_SUPER_RW_USER_NONE 4
#define MPU_DATA_PERM_SUPER_RW_USER_RD 5
#define MPU_DATA_PERM_SUPER_RW_USER_RW 6
#define MPU_DATA_PERM_SZ 3
#define MPU_DATA_PERM_MASK 0x7
#define MPU_INST_PERM_SUPER_EXEC_USER_EXEC 2
#define MPU_INST_PERM_SUPER_EXEC_USER_NONE 1
#define MPU_INST_PERM_SUPER_NONE_USER_NONE 0
#define MPU_INST_PERM_SZ 2
#define MPU_INST_PERM_MASK 0x3
#define MPU_MAX_REGION_SIZE_LOG2 20
#define MPU_MAX_REGIONS 32
#define MPU_MIN_REGION_SIZE_LOG2 6
#define MPU_MIN_REGIONS 1
#define MPUACC_REG_C_LSB 5
#define MPUACC_REG_C_MSB 5
#define MPUACC_REG_C_SZ 1
#define MPUACC_REG_C_MASK 0x1
#define MPUACC_REG_LIMIT_LSB 6
#define MPUACC_REG_LIMIT_MSB 31
#define MPUACC_REG_LIMIT_SZ 26
#define MPUACC_REG_LIMIT_MASK 0x3ffffff
#define MPUACC_REG_LSB 0
#define MPUACC_REG_MASK_LSB 6
#define MPUACC_REG_MASK_MSB 30
#define MPUACC_REG_MASK_SZ 25
#define MPUACC_REG_MASK_MASK 0x1ffffff
#define MPUACC_REG_MSB 31
#define MPUACC_REG_PERM_LSB 2
#define MPUACC_REG_PERM_MSB 4
#define MPUACC_REG_PERM_SZ 3
#define MPUACC_REG_PERM_MASK 0x7
#define MPUACC_REG_RD_LSB 1
#define MPUACC_REG_RD_MSB 1
#define MPUACC_REG_RD_SZ 1
#define MPUACC_REG_RD_MASK 0x1
#define MPUACC_REG_REGNUM 15
#define MPUACC_REG_RSV1_LSB 31
#define MPUACC_REG_RSV1_MSB 31
#define MPUACC_REG_RSV1_SZ 1
#define MPUACC_REG_RSV1_MASK 0x1
#define MPUACC_REG_SZ 32
#define MPUACC_REG_MASK 0xffffffff
#define MPUACC_REG_WR_LSB 0
#define MPUACC_REG_WR_MSB 0
#define MPUACC_REG_WR_SZ 1
#define MPUACC_REG_WR_MASK 0x1
#define MPUBASE_REG_BASE_LSB 6
#define MPUBASE_REG_BASE_MSB 30
#define MPUBASE_REG_BASE_SZ 25
#define MPUBASE_REG_BASE_MASK 0x1ffffff
#define MPUBASE_REG_D_LSB 0
#define MPUBASE_REG_D_MSB 0
#define MPUBASE_REG_D_SZ 1
#define MPUBASE_REG_D_MASK 0x1
#define MPUBASE_REG_INDEX_LSB 1
#define MPUBASE_REG_INDEX_MSB 5
#define MPUBASE_REG_INDEX_SZ 5
#define MPUBASE_REG_INDEX_MASK 0x1f
#define MPUBASE_REG_LSB 0
#define MPUBASE_REG_MSB 31
#define MPUBASE_REG_REGNUM 14
#define MPUBASE_REG_RSV1_LSB 31
#define MPUBASE_REG_RSV1_MSB 31
#define MPUBASE_REG_RSV1_SZ 1
#define MPUBASE_REG_RSV1_MASK 0x1
#define MPUBASE_REG_SZ 32
#define MPUBASE_REG_MASK 0xffffffff
#define PTEADDR_REG_LSB 0
#define PTEADDR_REG_MSB 31
#define PTEADDR_REG_PTBASE_LSB 22
#define PTEADDR_REG_PTBASE_MSB 31
#define PTEADDR_REG_PTBASE_SZ 10
#define PTEADDR_REG_PTBASE_MASK 0x3ff
#define PTEADDR_REG_REGNUM 8
#define PTEADDR_REG_RSV_LSB 0
#define PTEADDR_REG_RSV_MSB 1
#define PTEADDR_REG_RSV_SZ 2
#define PTEADDR_REG_RSV_MASK 0x3
#define PTEADDR_REG_SZ 32
#define PTEADDR_REG_MASK 0xffffffff
#define PTEADDR_REG_VPN_LSB 2
#define PTEADDR_REG_VPN_MSB 21
#define PTEADDR_REG_VPN_SZ 20
#define PTEADDR_REG_VPN_MASK 0xfffff
#define REGNUM_SZ 5
#define REGNUM_MASK 0x1f
#define RETADDR_REGNUM 31
#define RF_ADDR_SZ 5
#define RF_ADDR_MASK 0x1f
#define RF_NUM_REG 32
#define SIM_REG_LSB 0
#define SIM_REG_MSB 2
#define SIM_REG_PERF_CNT_CLR_LSB 2
#define SIM_REG_PERF_CNT_CLR_MSB 2
#define SIM_REG_PERF_CNT_CLR_SZ 1
#define SIM_REG_PERF_CNT_CLR_MASK 0x1
#define SIM_REG_PERF_CNT_EN_LSB 1
#define SIM_REG_PERF_CNT_EN_MSB 1
#define SIM_REG_PERF_CNT_EN_SZ 1
#define SIM_REG_PERF_CNT_EN_MASK 0x1
#define SIM_REG_REGNUM 6
#define SIM_REG_SHOW_MMU_REGS_LSB 4
#define SIM_REG_SHOW_MMU_REGS_MSB 4
#define SIM_REG_SHOW_MMU_REGS_SZ 1
#define SIM_REG_SHOW_MMU_REGS_MASK 0x1
#define SIM_REG_SHOW_TLB_LSB 3
#define SIM_REG_SHOW_TLB_MSB 3
#define SIM_REG_SHOW_TLB_SZ 1
#define SIM_REG_SHOW_TLB_MASK 0x1
#define SIM_REG_STOP_LSB 0
#define SIM_REG_STOP_MSB 0
#define SIM_REG_STOP_SZ 1
#define SIM_REG_STOP_MASK 0x1
#define SIM_REG_SZ 3
#define SIM_REG_MASK 0x7
#define SP_REGNUM 27
#define STATUS_REG_EH_LSB 2
#define STATUS_REG_EH_MSB 2
#define STATUS_REG_EH_SZ 1
#define STATUS_REG_EH_MASK 0x1
#define STATUS_REG_LSB 0
#define STATUS_REG_MMU_LSB 0
#define STATUS_REG_MMU_MSB 2
#define STATUS_REG_MMU_RSV_LSB 3
#define STATUS_REG_MMU_RSV_MSB 31
#define STATUS_REG_MMU_RSV_SZ 29
#define STATUS_REG_MMU_RSV_MASK 0x1fffffff
#define STATUS_REG_MMU_SZ 3
#define STATUS_REG_MMU_MASK 0x7
#define STATUS_REG_MPU_LSB 0
#define STATUS_REG_MPU_MSB 1
#define STATUS_REG_MPU_RSV_LSB 2
#define STATUS_REG_MPU_RSV_MSB 31
#define STATUS_REG_MPU_RSV_SZ 30
#define STATUS_REG_MPU_RSV_MASK 0x3fffffff
#define STATUS_REG_MPU_SZ 2
#define STATUS_REG_MPU_MASK 0x3
#define STATUS_REG_MSB 2
#define STATUS_REG_NO_MMU_LSB 0
#define STATUS_REG_NO_MMU_MSB 0
#define STATUS_REG_NO_MMU_RSV_LSB 1
#define STATUS_REG_NO_MMU_RSV_MSB 31
#define STATUS_REG_NO_MMU_RSV_SZ 31
#define STATUS_REG_NO_MMU_RSV_MASK 0x7fffffff
#define STATUS_REG_NO_MMU_SZ 1
#define STATUS_REG_NO_MMU_MASK 0x1
#define STATUS_REG_PIE_LSB 0
#define STATUS_REG_PIE_MSB 0
#define STATUS_REG_PIE_SZ 1
#define STATUS_REG_PIE_MASK 0x1
#define STATUS_REG_REGNUM 0
#define STATUS_REG_SZ 3
#define STATUS_REG_MASK 0x7
#define STATUS_REG_U_LSB 1
#define STATUS_REG_U_MSB 1
#define STATUS_REG_U_SZ 1
#define STATUS_REG_U_MASK 0x1
#define TLB_MAX_ENTRIES 1024
#define TLB_MAX_LINES 128
#define TLB_MAX_PID_SZ 14
#define TLB_MAX_PID_MASK 0x3fff
#define TLB_MAX_PTR_SZ 10
#define TLB_MAX_PTR_MASK 0x3ff
#define TLB_MAX_WAYS 16
#define TLB_MIN_PID_SZ 8
#define TLB_MIN_PID_MASK 0xff
#define TLB_MIN_PTR_SZ 7
#define TLB_MIN_PTR_MASK 0x7f
#define TLB_MIN_WAYS 8
#define TLBACC_REG_C_LSB 24
#define TLBACC_REG_C_MSB 24
#define TLBACC_REG_C_SZ 1
#define TLBACC_REG_C_MASK 0x1
#define TLBACC_REG_G_LSB 20
#define TLBACC_REG_G_MSB 20
#define TLBACC_REG_G_SZ 1
#define TLBACC_REG_G_MASK 0x1
#define TLBACC_REG_IG_LSB 25
#define TLBACC_REG_IG_MSB 31
#define TLBACC_REG_IG_SZ 7
#define TLBACC_REG_IG_MASK 0x7f
#define TLBACC_REG_LSB 0
#define TLBACC_REG_MSB 24
#define TLBACC_REG_PFN_LSB 0
#define TLBACC_REG_PFN_MSB 19
#define TLBACC_REG_PFN_SZ 20
#define TLBACC_REG_PFN_MASK 0xfffff
#define TLBACC_REG_R_LSB 23
#define TLBACC_REG_R_MSB 23
#define TLBACC_REG_R_SZ 1
#define TLBACC_REG_R_MASK 0x1
#define TLBACC_REG_REGNUM 9
#define TLBACC_REG_SZ 25
#define TLBACC_REG_MASK 0x1ffffff
#define TLBACC_REG_W_LSB 22
#define TLBACC_REG_W_MSB 22
#define TLBACC_REG_W_SZ 1
#define TLBACC_REG_W_MASK 0x1
#define TLBACC_REG_X_LSB 21
#define TLBACC_REG_X_MSB 21
#define TLBACC_REG_X_SZ 1
#define TLBACC_REG_X_MASK 0x1
#define TLBMISC_REG_BAD_LSB 2
#define TLBMISC_REG_BAD_MSB 2
#define TLBMISC_REG_BAD_SZ 1
#define TLBMISC_REG_BAD_MASK 0x1
#define TLBMISC_REG_D_LSB 0
#define TLBMISC_REG_D_MSB 0
#define TLBMISC_REG_D_SZ 1
#define TLBMISC_REG_D_MASK 0x1
#define TLBMISC_REG_DBL_LSB 3
#define TLBMISC_REG_DBL_MSB 3
#define TLBMISC_REG_DBL_SZ 1
#define TLBMISC_REG_DBL_MASK 0x1
#define TLBMISC_REG_LSB 0
#define TLBMISC_REG_MSB 23
#define TLBMISC_REG_PERM_LSB 1
#define TLBMISC_REG_PERM_MSB 1
#define TLBMISC_REG_PERM_SZ 1
#define TLBMISC_REG_PERM_MASK 0x1
#define TLBMISC_REG_PID_LSB 4
#define TLBMISC_REG_PID_MSB 17
#define TLBMISC_REG_PID_SZ 14
#define TLBMISC_REG_PID_MASK 0x3fff
#define TLBMISC_REG_RD_LSB 19
#define TLBMISC_REG_RD_MSB 19
#define TLBMISC_REG_RD_SZ 1
#define TLBMISC_REG_RD_MASK 0x1
#define TLBMISC_REG_REGNUM 10
#define TLBMISC_REG_RSV1_LSB 24
#define TLBMISC_REG_RSV1_MSB 31
#define TLBMISC_REG_RSV1_SZ 8
#define TLBMISC_REG_RSV1_MASK 0xff
#define TLBMISC_REG_SZ 24
#define TLBMISC_REG_MASK 0xffffff
#define TLBMISC_REG_WAY_LSB 20
#define TLBMISC_REG_WAY_MSB 23
#define TLBMISC_REG_WAY_SZ 4
#define TLBMISC_REG_WAY_MASK 0xf
#define TLBMISC_REG_WE_LSB 18
#define TLBMISC_REG_WE_MSB 18
#define TLBMISC_REG_WE_SZ 1
#define TLBMISC_REG_WE_MASK 0x1

/* Macros to extract instruction fields */
#define GET_IW_A(Iw) \
    (((Iw) >> IW_A_LSB) & IW_A_MASK)
#define SET_IW_A(Iw, Val) \
    Iw = (((Iw) & (~(IW_A_MASK << IW_A_LSB))) | \
         (((Val) & IW_A_MASK) << IW_A_LSB))
#define GET_IW_B(Iw) \
    (((Iw) >> IW_B_LSB) & IW_B_MASK)
#define SET_IW_B(Iw, Val) \
    Iw = (((Iw) & (~(IW_B_MASK << IW_B_LSB))) | \
         (((Val) & IW_B_MASK) << IW_B_LSB))
#define GET_IW_C(Iw) \
    (((Iw) >> IW_C_LSB) & IW_C_MASK)
#define SET_IW_C(Iw, Val) \
    Iw = (((Iw) & (~(IW_C_MASK << IW_C_LSB))) | \
         (((Val) & IW_C_MASK) << IW_C_LSB))
#define GET_IW_CONTROL_REGNUM(Iw) \
    (((Iw) >> IW_CONTROL_REGNUM_LSB) & IW_CONTROL_REGNUM_MASK)
#define SET_IW_CONTROL_REGNUM(Iw, Val) \
    Iw = (((Iw) & (~(IW_CONTROL_REGNUM_MASK << IW_CONTROL_REGNUM_LSB))) | \
         (((Val) & IW_CONTROL_REGNUM_MASK) << IW_CONTROL_REGNUM_LSB))
#define GET_IW_CUSTOM_N(Iw) \
    (((Iw) >> IW_CUSTOM_N_LSB) & IW_CUSTOM_N_MASK)
#define SET_IW_CUSTOM_N(Iw, Val) \
    Iw = (((Iw) & (~(IW_CUSTOM_N_MASK << IW_CUSTOM_N_LSB))) | \
         (((Val) & IW_CUSTOM_N_MASK) << IW_CUSTOM_N_LSB))
#define GET_IW_CUSTOM_READRA(Iw) \
    (((Iw) >> IW_CUSTOM_READRA_LSB) & IW_CUSTOM_READRA_MASK)
#define SET_IW_CUSTOM_READRA(Iw, Val) \
    Iw = (((Iw) & (~(IW_CUSTOM_READRA_MASK << IW_CUSTOM_READRA_LSB))) | \
         (((Val) & IW_CUSTOM_READRA_MASK) << IW_CUSTOM_READRA_LSB))
#define GET_IW_CUSTOM_READRB(Iw) \
    (((Iw) >> IW_CUSTOM_READRB_LSB) & IW_CUSTOM_READRB_MASK)
#define SET_IW_CUSTOM_READRB(Iw, Val) \
    Iw = (((Iw) & (~(IW_CUSTOM_READRB_MASK << IW_CUSTOM_READRB_LSB))) | \
         (((Val) & IW_CUSTOM_READRB_MASK) << IW_CUSTOM_READRB_LSB))
#define GET_IW_CUSTOM_WRITERC(Iw) \
    (((Iw) >> IW_CUSTOM_WRITERC_LSB) & IW_CUSTOM_WRITERC_MASK)
#define SET_IW_CUSTOM_WRITERC(Iw, Val) \
    Iw = (((Iw) & (~(IW_CUSTOM_WRITERC_MASK << IW_CUSTOM_WRITERC_LSB))) | \
         (((Val) & IW_CUSTOM_WRITERC_MASK) << IW_CUSTOM_WRITERC_LSB))
#define GET_IW_IMM16(Iw) \
    (((Iw) >> IW_IMM16_LSB) & IW_IMM16_MASK)
#define SET_IW_IMM16(Iw, Val) \
    Iw = (((Iw) & (~(IW_IMM16_MASK << IW_IMM16_LSB))) | \
         (((Val) & IW_IMM16_MASK) << IW_IMM16_LSB))
#define GET_IW_IMM26(Iw) \
    (((Iw) >> IW_IMM26_LSB) & IW_IMM26_MASK)
#define SET_IW_IMM26(Iw, Val) \
    Iw = (((Iw) & (~(IW_IMM26_MASK << IW_IMM26_LSB))) | \
         (((Val) & IW_IMM26_MASK) << IW_IMM26_LSB))
#define GET_IW_MEMSZ(Iw) \
    (((Iw) >> IW_MEMSZ_LSB) & IW_MEMSZ_MASK)
#define SET_IW_MEMSZ(Iw, Val) \
    Iw = (((Iw) & (~(IW_MEMSZ_MASK << IW_MEMSZ_LSB))) | \
         (((Val) & IW_MEMSZ_MASK) << IW_MEMSZ_LSB))
#define GET_IW_OP(Iw) \
    (((Iw) >> IW_OP_LSB) & IW_OP_MASK)
#define SET_IW_OP(Iw, Val) \
    Iw = (((Iw) & (~(IW_OP_MASK << IW_OP_LSB))) | \
         (((Val) & IW_OP_MASK) << IW_OP_LSB))
#define GET_IW_OPX(Iw) \
    (((Iw) >> IW_OPX_LSB) & IW_OPX_MASK)
#define SET_IW_OPX(Iw, Val) \
    Iw = (((Iw) & (~(IW_OPX_MASK << IW_OPX_LSB))) | \
         (((Val) & IW_OPX_MASK) << IW_OPX_LSB))
#define GET_IW_SHIFT_IMM5(Iw) \
    (((Iw) >> IW_SHIFT_IMM5_LSB) & IW_SHIFT_IMM5_MASK)
#define SET_IW_SHIFT_IMM5(Iw, Val) \
    Iw = (((Iw) & (~(IW_SHIFT_IMM5_MASK << IW_SHIFT_IMM5_LSB))) | \
         (((Val) & IW_SHIFT_IMM5_MASK) << IW_SHIFT_IMM5_LSB))
#define GET_IW_TRAP_BREAK_IMM5(Iw) \
    (((Iw) >> IW_TRAP_BREAK_IMM5_LSB) & IW_TRAP_BREAK_IMM5_MASK)
#define SET_IW_TRAP_BREAK_IMM5(Iw, Val) \
    Iw = (((Iw) & (~(IW_TRAP_BREAK_IMM5_MASK << IW_TRAP_BREAK_IMM5_LSB))) | \
         (((Val) & IW_TRAP_BREAK_IMM5_MASK) << IW_TRAP_BREAK_IMM5_LSB))

/* Macros to extract control register fields */
#define GET_BADADDR_REG_BADDR(Reg) \
    (((Reg) >> BADADDR_REG_BADDR_LSB) & BADADDR_REG_BADDR_MASK)
#define SET_BADADDR_REG_BADDR(Reg, Val) \
    Reg = (((Reg) & (~(BADADDR_REG_BADDR_MASK << BADADDR_REG_BADDR_LSB))) | \
         (((Val) & BADADDR_REG_BADDR_MASK) << BADADDR_REG_BADDR_LSB))
#define GET_BSTATUS_REG_MMU(Reg) \
    (((Reg) >> BSTATUS_REG_MMU_LSB) & BSTATUS_REG_MMU_MASK)
#define SET_BSTATUS_REG_MMU(Reg, Val) \
    Reg = (((Reg) & (~(BSTATUS_REG_MMU_MASK << BSTATUS_REG_MMU_LSB))) | \
         (((Val) & BSTATUS_REG_MMU_MASK) << BSTATUS_REG_MMU_LSB))
#define GET_BSTATUS_REG_MPU(Reg) \
    (((Reg) >> BSTATUS_REG_MPU_LSB) & BSTATUS_REG_MPU_MASK)
#define SET_BSTATUS_REG_MPU(Reg, Val) \
    Reg = (((Reg) & (~(BSTATUS_REG_MPU_MASK << BSTATUS_REG_MPU_LSB))) | \
         (((Val) & BSTATUS_REG_MPU_MASK) << BSTATUS_REG_MPU_LSB))
#define GET_BSTATUS_REG_NO_MMU(Reg) \
    (((Reg) >> BSTATUS_REG_NO_MMU_LSB) & BSTATUS_REG_NO_MMU_MASK)
#define SET_BSTATUS_REG_NO_MMU(Reg, Val) \
    Reg = (((Reg) & (~(BSTATUS_REG_NO_MMU_MASK << BSTATUS_REG_NO_MMU_LSB))) | \
         (((Val) & BSTATUS_REG_NO_MMU_MASK) << BSTATUS_REG_NO_MMU_LSB))
#define GET_CONFIG_REG_PE(Reg) \
    (((Reg) >> CONFIG_REG_PE_LSB) & CONFIG_REG_PE_MASK)
#define SET_CONFIG_REG_PE(Reg, Val) \
    Reg = (((Reg) & (~(CONFIG_REG_PE_MASK << CONFIG_REG_PE_LSB))) | \
         (((Val) & CONFIG_REG_PE_MASK) << CONFIG_REG_PE_LSB))
#define GET_ESTATUS_REG_MMU(Reg) \
    (((Reg) >> ESTATUS_REG_MMU_LSB) & ESTATUS_REG_MMU_MASK)
#define SET_ESTATUS_REG_MMU(Reg, Val) \
    Reg = (((Reg) & (~(ESTATUS_REG_MMU_MASK << ESTATUS_REG_MMU_LSB))) | \
         (((Val) & ESTATUS_REG_MMU_MASK) << ESTATUS_REG_MMU_LSB))
#define GET_ESTATUS_REG_MPU(Reg) \
    (((Reg) >> ESTATUS_REG_MPU_LSB) & ESTATUS_REG_MPU_MASK)
#define SET_ESTATUS_REG_MPU(Reg, Val) \
    Reg = (((Reg) & (~(ESTATUS_REG_MPU_MASK << ESTATUS_REG_MPU_LSB))) | \
         (((Val) & ESTATUS_REG_MPU_MASK) << ESTATUS_REG_MPU_LSB))
#define GET_ESTATUS_REG_NO_MMU(Reg) \
    (((Reg) >> ESTATUS_REG_NO_MMU_LSB) & ESTATUS_REG_NO_MMU_MASK)
#define SET_ESTATUS_REG_NO_MMU(Reg, Val) \
    Reg = (((Reg) & (~(ESTATUS_REG_NO_MMU_MASK << ESTATUS_REG_NO_MMU_LSB))) | \
         (((Val) & ESTATUS_REG_NO_MMU_MASK) << ESTATUS_REG_NO_MMU_LSB))
#define GET_EXCEPTION_REG_CAUSE(Reg) \
    (((Reg) >> EXCEPTION_REG_CAUSE_LSB) & EXCEPTION_REG_CAUSE_MASK)
#define SET_EXCEPTION_REG_CAUSE(Reg, Val) \
    Reg = (((Reg) & (~(EXCEPTION_REG_CAUSE_MASK << \
                       EXCEPTION_REG_CAUSE_LSB))) | \
         (((Val) & EXCEPTION_REG_CAUSE_MASK) << EXCEPTION_REG_CAUSE_LSB))
#define GET_EXCEPTION_REG_MEA(Reg) \
    (((Reg) >> EXCEPTION_REG_MEA_LSB) & EXCEPTION_REG_MEA_MASK)
#define SET_EXCEPTION_REG_MEA(Reg, Val) \
    Reg = (((Reg) & (~(EXCEPTION_REG_MEA_MASK << EXCEPTION_REG_MEA_LSB))) | \
         (((Val) & EXCEPTION_REG_MEA_MASK) << EXCEPTION_REG_MEA_LSB))
#define GET_EXCEPTION_REG_MEE(Reg) \
    (((Reg) >> EXCEPTION_REG_MEE_LSB) & EXCEPTION_REG_MEE_MASK)
#define SET_EXCEPTION_REG_MEE(Reg, Val) \
    Reg = (((Reg) & (~(EXCEPTION_REG_MEE_MASK << EXCEPTION_REG_MEE_LSB))) | \
         (((Val) & EXCEPTION_REG_MEE_MASK) << EXCEPTION_REG_MEE_LSB))
#define GET_MPUACC_REG_C(Reg) \
    (((Reg) >> MPUACC_REG_C_LSB) & MPUACC_REG_C_MASK)
#define SET_MPUACC_REG_C(Reg, Val) \
    Reg = (((Reg) & (~(MPUACC_REG_C_MASK << MPUACC_REG_C_LSB))) | \
         (((Val) & MPUACC_REG_C_MASK) << MPUACC_REG_C_LSB))
#define GET_MPUACC_REG_LIMIT(Reg) \
    (((Reg) >> MPUACC_REG_LIMIT_LSB) & MPUACC_REG_LIMIT_MASK)
#define SET_MPUACC_REG_LIMIT(Reg, Val) \
    Reg = (((Reg) & (~(MPUACC_REG_LIMIT_MASK << MPUACC_REG_LIMIT_LSB))) | \
         (((Val) & MPUACC_REG_LIMIT_MASK) << MPUACC_REG_LIMIT_LSB))
#define GET_MPUACC_REG_MASK(Reg) \
    (((Reg) >> MPUACC_REG_MASK_LSB) & MPUACC_REG_MASK_MASK)
#define SET_MPUACC_REG_MASK(Reg, Val) \
    Reg = (((Reg) & (~(MPUACC_REG_MASK_MASK << MPUACC_REG_MASK_LSB))) | \
         (((Val) & MPUACC_REG_MASK_MASK) << MPUACC_REG_MASK_LSB))
#define GET_MPUACC_REG_PERM(Reg) \
    (((Reg) >> MPUACC_REG_PERM_LSB) & MPUACC_REG_PERM_MASK)
#define SET_MPUACC_REG_PERM(Reg, Val) \
    Reg = (((Reg) & (~(MPUACC_REG_PERM_MASK << MPUACC_REG_PERM_LSB))) | \
         (((Val) & MPUACC_REG_PERM_MASK) << MPUACC_REG_PERM_LSB))
#define GET_MPUACC_REG_RD(Reg) \
    (((Reg) >> MPUACC_REG_RD_LSB) & MPUACC_REG_RD_MASK)
#define SET_MPUACC_REG_RD(Reg, Val) \
    Reg = (((Reg) & (~(MPUACC_REG_RD_MASK << MPUACC_REG_RD_LSB))) | \
         (((Val) & MPUACC_REG_RD_MASK) << MPUACC_REG_RD_LSB))
#define GET_MPUACC_REG_RSV1(Reg) \
    (((Reg) >> MPUACC_REG_RSV1_LSB) & MPUACC_REG_RSV1_MASK)
#define SET_MPUACC_REG_RSV1(Reg, Val) \
    Reg = (((Reg) & (~(MPUACC_REG_RSV1_MASK << MPUACC_REG_RSV1_LSB))) | \
         (((Val) & MPUACC_REG_RSV1_MASK) << MPUACC_REG_RSV1_LSB))
#define GET_MPUACC_REG_WR(Reg) \
    (((Reg) >> MPUACC_REG_WR_LSB) & MPUACC_REG_WR_MASK)
#define SET_MPUACC_REG_WR(Reg, Val) \
    Reg = (((Reg) & (~(MPUACC_REG_WR_MASK << MPUACC_REG_WR_LSB))) | \
         (((Val) & MPUACC_REG_WR_MASK) << MPUACC_REG_WR_LSB))
#define GET_MPUBASE_REG_BASE(Reg) \
    (((Reg) >> MPUBASE_REG_BASE_LSB) & MPUBASE_REG_BASE_MASK)
#define SET_MPUBASE_REG_BASE(Reg, Val) \
    Reg = (((Reg) & (~(MPUBASE_REG_BASE_MASK << MPUBASE_REG_BASE_LSB))) | \
         (((Val) & MPUBASE_REG_BASE_MASK) << MPUBASE_REG_BASE_LSB))
#define GET_MPUBASE_REG_D(Reg) \
    (((Reg) >> MPUBASE_REG_D_LSB) & MPUBASE_REG_D_MASK)
#define SET_MPUBASE_REG_D(Reg, Val) \
    Reg = (((Reg) & (~(MPUBASE_REG_D_MASK << MPUBASE_REG_D_LSB))) | \
         (((Val) & MPUBASE_REG_D_MASK) << MPUBASE_REG_D_LSB))
#define GET_MPUBASE_REG_INDEX(Reg) \
    (((Reg) >> MPUBASE_REG_INDEX_LSB) & MPUBASE_REG_INDEX_MASK)
#define SET_MPUBASE_REG_INDEX(Reg, Val) \
    Reg = (((Reg) & (~(MPUBASE_REG_INDEX_MASK << MPUBASE_REG_INDEX_LSB))) | \
         (((Val) & MPUBASE_REG_INDEX_MASK) << MPUBASE_REG_INDEX_LSB))
#define GET_MPUBASE_REG_RSV1(Reg) \
    (((Reg) >> MPUBASE_REG_RSV1_LSB) & MPUBASE_REG_RSV1_MASK)
#define SET_MPUBASE_REG_RSV1(Reg, Val) \
    Reg = (((Reg) & (~(MPUBASE_REG_RSV1_MASK << MPUBASE_REG_RSV1_LSB))) | \
         (((Val) & MPUBASE_REG_RSV1_MASK) << MPUBASE_REG_RSV1_LSB))
#define GET_PTEADDR_REG_PTBASE(Reg) \
    (((Reg) >> PTEADDR_REG_PTBASE_LSB) & PTEADDR_REG_PTBASE_MASK)
#define SET_PTEADDR_REG_PTBASE(Reg, Val) \
    Reg = (((Reg) & (~(PTEADDR_REG_PTBASE_MASK << PTEADDR_REG_PTBASE_LSB))) | \
         (((Val) & PTEADDR_REG_PTBASE_MASK) << PTEADDR_REG_PTBASE_LSB))
#define GET_PTEADDR_REG_RSV(Reg) \
    (((Reg) >> PTEADDR_REG_RSV_LSB) & PTEADDR_REG_RSV_MASK)
#define SET_PTEADDR_REG_RSV(Reg, Val) \
    Reg = (((Reg) & (~(PTEADDR_REG_RSV_MASK << PTEADDR_REG_RSV_LSB))) | \
         (((Val) & PTEADDR_REG_RSV_MASK) << PTEADDR_REG_RSV_LSB))
#define GET_PTEADDR_REG_VPN(Reg) \
    (((Reg) >> PTEADDR_REG_VPN_LSB) & PTEADDR_REG_VPN_MASK)
#define SET_PTEADDR_REG_VPN(Reg, Val) \
    Reg = (((Reg) & (~(PTEADDR_REG_VPN_MASK << PTEADDR_REG_VPN_LSB))) | \
         (((Val) & PTEADDR_REG_VPN_MASK) << PTEADDR_REG_VPN_LSB))
#define GET_SIM_REG_PERF_CNT_CLR(Reg) \
    (((Reg) >> SIM_REG_PERF_CNT_CLR_LSB) & SIM_REG_PERF_CNT_CLR_MASK)
#define SET_SIM_REG_PERF_CNT_CLR(Reg, Val) \
    Reg = (((Reg) & (~(SIM_REG_PERF_CNT_CLR_MASK << \
                       SIM_REG_PERF_CNT_CLR_LSB))) | \
         (((Val) & SIM_REG_PERF_CNT_CLR_MASK) << SIM_REG_PERF_CNT_CLR_LSB))
#define GET_SIM_REG_PERF_CNT_EN(Reg) \
    (((Reg) >> SIM_REG_PERF_CNT_EN_LSB) & SIM_REG_PERF_CNT_EN_MASK)
#define SET_SIM_REG_PERF_CNT_EN(Reg, Val) \
    Reg = (((Reg) & (~(SIM_REG_PERF_CNT_EN_MASK << \
                       SIM_REG_PERF_CNT_EN_LSB))) | \
         (((Val) & SIM_REG_PERF_CNT_EN_MASK) << SIM_REG_PERF_CNT_EN_LSB))
#define GET_SIM_REG_SHOW_MMU_REGS(Reg) \
    (((Reg) >> SIM_REG_SHOW_MMU_REGS_LSB) & SIM_REG_SHOW_MMU_REGS_MASK)
#define SET_SIM_REG_SHOW_MMU_REGS(Reg, Val) \
    Reg = (((Reg) & (~(SIM_REG_SHOW_MMU_REGS_MASK << \
                       SIM_REG_SHOW_MMU_REGS_LSB))) | \
         (((Val) & SIM_REG_SHOW_MMU_REGS_MASK) << SIM_REG_SHOW_MMU_REGS_LSB))
#define GET_SIM_REG_SHOW_TLB(Reg) \
    (((Reg) >> SIM_REG_SHOW_TLB_LSB) & SIM_REG_SHOW_TLB_MASK)
#define SET_SIM_REG_SHOW_TLB(Reg, Val) \
    Reg = (((Reg) & (~(SIM_REG_SHOW_TLB_MASK << SIM_REG_SHOW_TLB_LSB))) | \
         (((Val) & SIM_REG_SHOW_TLB_MASK) << SIM_REG_SHOW_TLB_LSB))
#define GET_SIM_REG_STOP(Reg) \
    (((Reg) >> SIM_REG_STOP_LSB) & SIM_REG_STOP_MASK)
#define SET_SIM_REG_STOP(Reg, Val) \
    Reg = (((Reg) & (~(SIM_REG_STOP_MASK << SIM_REG_STOP_LSB))) | \
         (((Val) & SIM_REG_STOP_MASK) << SIM_REG_STOP_LSB))
#define GET_STATUS_REG_EH(Reg) \
    (((Reg) >> STATUS_REG_EH_LSB) & STATUS_REG_EH_MASK)
#define SET_STATUS_REG_EH(Reg, Val) \
    Reg = (((Reg) & (~(STATUS_REG_EH_MASK << STATUS_REG_EH_LSB))) | \
         (((Val) & STATUS_REG_EH_MASK) << STATUS_REG_EH_LSB))
#define GET_STATUS_REG_MMU(Reg) \
    (((Reg) >> STATUS_REG_MMU_LSB) & STATUS_REG_MMU_MASK)
#define SET_STATUS_REG_MMU(Reg, Val) \
    Reg = (((Reg) & (~(STATUS_REG_MMU_MASK << STATUS_REG_MMU_LSB))) | \
         (((Val) & STATUS_REG_MMU_MASK) << STATUS_REG_MMU_LSB))
#define GET_STATUS_REG_MMU_RSV(Reg) \
    (((Reg) >> STATUS_REG_MMU_RSV_LSB) & STATUS_REG_MMU_RSV_MASK)
#define SET_STATUS_REG_MMU_RSV(Reg, Val) \
    Reg = (((Reg) & (~(STATUS_REG_MMU_RSV_MASK << STATUS_REG_MMU_RSV_LSB))) | \
         (((Val) & STATUS_REG_MMU_RSV_MASK) << STATUS_REG_MMU_RSV_LSB))
#define GET_STATUS_REG_MPU(Reg) \
    (((Reg) >> STATUS_REG_MPU_LSB) & STATUS_REG_MPU_MASK)
#define SET_STATUS_REG_MPU(Reg, Val) \
    Reg = (((Reg) & (~(STATUS_REG_MPU_MASK << STATUS_REG_MPU_LSB))) | \
         (((Val) & STATUS_REG_MPU_MASK) << STATUS_REG_MPU_LSB))
#define GET_STATUS_REG_MPU_RSV(Reg) \
    (((Reg) >> STATUS_REG_MPU_RSV_LSB) & STATUS_REG_MPU_RSV_MASK)
#define SET_STATUS_REG_MPU_RSV(Reg, Val) \
    Reg = (((Reg) & (~(STATUS_REG_MPU_RSV_MASK << STATUS_REG_MPU_RSV_LSB))) | \
         (((Val) & STATUS_REG_MPU_RSV_MASK) << STATUS_REG_MPU_RSV_LSB))
#define GET_STATUS_REG_NO_MMU(Reg) \
    (((Reg) >> STATUS_REG_NO_MMU_LSB) & STATUS_REG_NO_MMU_MASK)
#define SET_STATUS_REG_NO_MMU(Reg, Val) \
    Reg = (((Reg) & (~(STATUS_REG_NO_MMU_MASK << STATUS_REG_NO_MMU_LSB))) | \
         (((Val) & STATUS_REG_NO_MMU_MASK) << STATUS_REG_NO_MMU_LSB))
#define GET_STATUS_REG_NO_MMU_RSV(Reg) \
    (((Reg) >> STATUS_REG_NO_MMU_RSV_LSB) & STATUS_REG_NO_MMU_RSV_MASK)
#define SET_STATUS_REG_NO_MMU_RSV(Reg, Val) \
    Reg = (((Reg) & (~(STATUS_REG_NO_MMU_RSV_MASK << \
                       STATUS_REG_NO_MMU_RSV_LSB))) | \
         (((Val) & STATUS_REG_NO_MMU_RSV_MASK) << STATUS_REG_NO_MMU_RSV_LSB))
#define GET_STATUS_REG_PIE(Reg) \
    (((Reg) >> STATUS_REG_PIE_LSB) & STATUS_REG_PIE_MASK)
#define SET_STATUS_REG_PIE(Reg, Val) \
    Reg = (((Reg) & (~(STATUS_REG_PIE_MASK << STATUS_REG_PIE_LSB))) | \
         (((Val) & STATUS_REG_PIE_MASK) << STATUS_REG_PIE_LSB))
#define GET_STATUS_REG_U(Reg) \
    (((Reg) >> STATUS_REG_U_LSB) & STATUS_REG_U_MASK)
#define SET_STATUS_REG_U(Reg, Val) \
    Reg = (((Reg) & (~(STATUS_REG_U_MASK << STATUS_REG_U_LSB))) | \
         (((Val) & STATUS_REG_U_MASK) << STATUS_REG_U_LSB))
#define GET_TLBACC_REG_C(Reg) \
    (((Reg) >> TLBACC_REG_C_LSB) & TLBACC_REG_C_MASK)
#define SET_TLBACC_REG_C(Reg, Val) \
    Reg = (((Reg) & (~(TLBACC_REG_C_MASK << TLBACC_REG_C_LSB))) | \
         (((Val) & TLBACC_REG_C_MASK) << TLBACC_REG_C_LSB))
#define GET_TLBACC_REG_G(Reg) \
    (((Reg) >> TLBACC_REG_G_LSB) & TLBACC_REG_G_MASK)
#define SET_TLBACC_REG_G(Reg, Val) \
    Reg = (((Reg) & (~(TLBACC_REG_G_MASK << TLBACC_REG_G_LSB))) | \
         (((Val) & TLBACC_REG_G_MASK) << TLBACC_REG_G_LSB))
#define GET_TLBACC_REG_IG(Reg) \
    (((Reg) >> TLBACC_REG_IG_LSB) & TLBACC_REG_IG_MASK)
#define SET_TLBACC_REG_IG(Reg, Val) \
    Reg = (((Reg) & (~(TLBACC_REG_IG_MASK << TLBACC_REG_IG_LSB))) | \
         (((Val) & TLBACC_REG_IG_MASK) << TLBACC_REG_IG_LSB))
#define GET_TLBACC_REG_PFN(Reg) \
    (((Reg) >> TLBACC_REG_PFN_LSB) & TLBACC_REG_PFN_MASK)
#define SET_TLBACC_REG_PFN(Reg, Val) \
    Reg = (((Reg) & (~(TLBACC_REG_PFN_MASK << TLBACC_REG_PFN_LSB))) | \
         (((Val) & TLBACC_REG_PFN_MASK) << TLBACC_REG_PFN_LSB))
#define GET_TLBACC_REG_R(Reg) \
    (((Reg) >> TLBACC_REG_R_LSB) & TLBACC_REG_R_MASK)
#define SET_TLBACC_REG_R(Reg, Val) \
    Reg = (((Reg) & (~(TLBACC_REG_R_MASK << TLBACC_REG_R_LSB))) | \
         (((Val) & TLBACC_REG_R_MASK) << TLBACC_REG_R_LSB))
#define GET_TLBACC_REG_W(Reg) \
    (((Reg) >> TLBACC_REG_W_LSB) & TLBACC_REG_W_MASK)
#define SET_TLBACC_REG_W(Reg, Val) \
    Reg = (((Reg) & (~(TLBACC_REG_W_MASK << TLBACC_REG_W_LSB))) | \
         (((Val) & TLBACC_REG_W_MASK) << TLBACC_REG_W_LSB))
#define GET_TLBACC_REG_X(Reg) \
    (((Reg) >> TLBACC_REG_X_LSB) & TLBACC_REG_X_MASK)
#define SET_TLBACC_REG_X(Reg, Val) \
    Reg = (((Reg) & (~(TLBACC_REG_X_MASK << TLBACC_REG_X_LSB))) | \
         (((Val) & TLBACC_REG_X_MASK) << TLBACC_REG_X_LSB))
#define GET_TLBMISC_REG_BAD(Reg) \
    (((Reg) >> TLBMISC_REG_BAD_LSB) & TLBMISC_REG_BAD_MASK)
#define SET_TLBMISC_REG_BAD(Reg, Val) \
    Reg = (((Reg) & (~(TLBMISC_REG_BAD_MASK << TLBMISC_REG_BAD_LSB))) | \
         (((Val) & TLBMISC_REG_BAD_MASK) << TLBMISC_REG_BAD_LSB))
#define GET_TLBMISC_REG_D(Reg) \
    (((Reg) >> TLBMISC_REG_D_LSB) & TLBMISC_REG_D_MASK)
#define SET_TLBMISC_REG_D(Reg, Val) \
    Reg = (((Reg) & (~(TLBMISC_REG_D_MASK << TLBMISC_REG_D_LSB))) | \
         (((Val) & TLBMISC_REG_D_MASK) << TLBMISC_REG_D_LSB))
#define GET_TLBMISC_REG_DBL(Reg) \
    (((Reg) >> TLBMISC_REG_DBL_LSB) & TLBMISC_REG_DBL_MASK)
#define SET_TLBMISC_REG_DBL(Reg, Val) \
    Reg = (((Reg) & (~(TLBMISC_REG_DBL_MASK << TLBMISC_REG_DBL_LSB))) | \
         (((Val) & TLBMISC_REG_DBL_MASK) << TLBMISC_REG_DBL_LSB))
#define GET_TLBMISC_REG_PERM(Reg) \
    (((Reg) >> TLBMISC_REG_PERM_LSB) & TLBMISC_REG_PERM_MASK)
#define SET_TLBMISC_REG_PERM(Reg, Val) \
    Reg = (((Reg) & (~(TLBMISC_REG_PERM_MASK << TLBMISC_REG_PERM_LSB))) | \
         (((Val) & TLBMISC_REG_PERM_MASK) << TLBMISC_REG_PERM_LSB))
#define GET_TLBMISC_REG_PID(Reg) \
    (((Reg) >> TLBMISC_REG_PID_LSB) & TLBMISC_REG_PID_MASK)
#define SET_TLBMISC_REG_PID(Reg, Val) \
    Reg = (((Reg) & (~(TLBMISC_REG_PID_MASK << TLBMISC_REG_PID_LSB))) | \
         (((Val) & TLBMISC_REG_PID_MASK) << TLBMISC_REG_PID_LSB))
#define GET_TLBMISC_REG_RD(Reg) \
    (((Reg) >> TLBMISC_REG_RD_LSB) & TLBMISC_REG_RD_MASK)
#define SET_TLBMISC_REG_RD(Reg, Val) \
    Reg = (((Reg) & (~(TLBMISC_REG_RD_MASK << TLBMISC_REG_RD_LSB))) | \
         (((Val) & TLBMISC_REG_RD_MASK) << TLBMISC_REG_RD_LSB))
#define GET_TLBMISC_REG_RSV1(Reg) \
    (((Reg) >> TLBMISC_REG_RSV1_LSB) & TLBMISC_REG_RSV1_MASK)
#define SET_TLBMISC_REG_RSV1(Reg, Val) \
    Reg = (((Reg) & (~(TLBMISC_REG_RSV1_MASK << TLBMISC_REG_RSV1_LSB))) | \
         (((Val) & TLBMISC_REG_RSV1_MASK) << TLBMISC_REG_RSV1_LSB))
#define GET_TLBMISC_REG_WAY(Reg) \
    (((Reg) >> TLBMISC_REG_WAY_LSB) & TLBMISC_REG_WAY_MASK)
#define SET_TLBMISC_REG_WAY(Reg, Val) \
    Reg = (((Reg) & (~(TLBMISC_REG_WAY_MASK << TLBMISC_REG_WAY_LSB))) | \
         (((Val) & TLBMISC_REG_WAY_MASK) << TLBMISC_REG_WAY_LSB))
#define GET_TLBMISC_REG_WE(Reg) \
    (((Reg) >> TLBMISC_REG_WE_LSB) & TLBMISC_REG_WE_MASK)
#define SET_TLBMISC_REG_WE(Reg, Val) \
    Reg = (((Reg) & (~(TLBMISC_REG_WE_MASK << TLBMISC_REG_WE_LSB))) | \
         (((Val) & TLBMISC_REG_WE_MASK) << TLBMISC_REG_WE_LSB))

/* Macros to extract MMU fields */
#define GET_MMU_ADDR_BYPASS_TLB_CACHEABLE(Addr) \
    (((Addr) >> MMU_ADDR_BYPASS_TLB_CACHEABLE_LSB) & \
     MMU_ADDR_BYPASS_TLB_CACHEABLE_MASK)
#define SET_MMU_ADDR_BYPASS_TLB_CACHEABLE(Addr, Val) \
    Addr = (((Addr) & (~(MMU_ADDR_BYPASS_TLB_CACHEABLE_MASK << \
                         MMU_ADDR_BYPASS_TLB_CACHEABLE_LSB))) | \
         (((Val) & MMU_ADDR_BYPASS_TLB_CACHEABLE_MASK) << \
          MMU_ADDR_BYPASS_TLB_CACHEABLE_LSB))
#define GET_MMU_ADDR_BYPASS_TLB(Addr) \
    (((Addr) >> MMU_ADDR_BYPASS_TLB_LSB) & MMU_ADDR_BYPASS_TLB_MASK)
#define SET_MMU_ADDR_BYPASS_TLB(Addr, Val) \
    Addr = (((Addr) & (~(MMU_ADDR_BYPASS_TLB_MASK << \
                         MMU_ADDR_BYPASS_TLB_LSB))) | \
         (((Val) & MMU_ADDR_BYPASS_TLB_MASK) << MMU_ADDR_BYPASS_TLB_LSB))
#define GET_MMU_ADDR_BYPASS_TLB_PADDR(Addr) \
    (((Addr) >> MMU_ADDR_BYPASS_TLB_PADDR_LSB) & MMU_ADDR_BYPASS_TLB_PADDR_MASK)
#define SET_MMU_ADDR_BYPASS_TLB_PADDR(Addr, Val) \
    Addr = (((Addr) & (~(MMU_ADDR_BYPASS_TLB_PADDR_MASK << \
                         MMU_ADDR_BYPASS_TLB_PADDR_LSB))) | \
         (((Val) & MMU_ADDR_BYPASS_TLB_PADDR_MASK) << \
          MMU_ADDR_BYPASS_TLB_PADDR_LSB))
#define GET_MMU_ADDR_IO_REGION(Addr) \
    (((Addr) >> MMU_ADDR_IO_REGION_LSB) & MMU_ADDR_IO_REGION_MASK)
#define SET_MMU_ADDR_IO_REGION(Addr, Val) \
    Addr = (((Addr) & (~(MMU_ADDR_IO_REGION_MASK << \
                         MMU_ADDR_IO_REGION_LSB))) | \
         (((Val) & MMU_ADDR_IO_REGION_MASK) << MMU_ADDR_IO_REGION_LSB))
#define GET_MMU_ADDR_KERNEL_MMU_REGION(Addr) \
    (((Addr) >> MMU_ADDR_KERNEL_MMU_REGION_LSB) & \
     MMU_ADDR_KERNEL_MMU_REGION_MASK)
#define SET_MMU_ADDR_KERNEL_MMU_REGION(Addr, Val) \
    Addr = (((Addr) & (~(MMU_ADDR_KERNEL_MMU_REGION_MASK << \
                         MMU_ADDR_KERNEL_MMU_REGION_LSB))) | \
         (((Val) & MMU_ADDR_KERNEL_MMU_REGION_MASK) << \
          MMU_ADDR_KERNEL_MMU_REGION_LSB))
#define GET_MMU_ADDR_KERNEL_REGION(Addr) \
    (((Addr) >> MMU_ADDR_KERNEL_REGION_LSB) & MMU_ADDR_KERNEL_REGION_MASK)
#define SET_MMU_ADDR_KERNEL_REGION(Addr, Val) \
    Addr = (((Addr) & (~(MMU_ADDR_KERNEL_REGION_MASK << \
                         MMU_ADDR_KERNEL_REGION_LSB))) | \
         (((Val) & MMU_ADDR_KERNEL_REGION_MASK) << MMU_ADDR_KERNEL_REGION_LSB))
#define GET_MMU_ADDR_PAGE_OFFSET(Addr) \
    (((Addr) >> MMU_ADDR_PAGE_OFFSET_LSB) & MMU_ADDR_PAGE_OFFSET_MASK)
#define SET_MMU_ADDR_PAGE_OFFSET(Addr, Val) \
    Addr = (((Addr) & (~(MMU_ADDR_PAGE_OFFSET_MASK << \
                         MMU_ADDR_PAGE_OFFSET_LSB))) | \
         (((Val) & MMU_ADDR_PAGE_OFFSET_MASK) << MMU_ADDR_PAGE_OFFSET_LSB))
#define GET_MMU_ADDR_PFN(Addr) \
    (((Addr) >> MMU_ADDR_PFN_LSB) & MMU_ADDR_PFN_MASK)
#define SET_MMU_ADDR_PFN(Addr, Val) \
    Addr = (((Addr) & (~(MMU_ADDR_PFN_MASK << MMU_ADDR_PFN_LSB))) | \
         (((Val) & MMU_ADDR_PFN_MASK) << MMU_ADDR_PFN_LSB))
#define GET_MMU_ADDR_USER_REGION(Addr) \
    (((Addr) >> MMU_ADDR_USER_REGION_LSB) & MMU_ADDR_USER_REGION_MASK)
#define SET_MMU_ADDR_USER_REGION(Addr, Val) \
    Addr = (((Addr) & (~(MMU_ADDR_USER_REGION_MASK << \
                         MMU_ADDR_USER_REGION_LSB))) | \
         (((Val) & MMU_ADDR_USER_REGION_MASK) << MMU_ADDR_USER_REGION_LSB))
#define GET_MMU_ADDR_VPN(Addr) \
    (((Addr) >> MMU_ADDR_VPN_LSB) & MMU_ADDR_VPN_MASK)
#define SET_MMU_ADDR_VPN(Addr, Val) \
    Addr = (((Addr) & (~(MMU_ADDR_VPN_MASK << MMU_ADDR_VPN_LSB))) | \
         (((Val) & MMU_ADDR_VPN_MASK) << MMU_ADDR_VPN_LSB))

/* OP instruction values */
#define OP_ADDI 4
#define OP_ANDHI 44
#define OP_ANDI 12
#define OP_BEQ 38
#define OP_BGE 14
#define OP_BGEU 46
#define OP_BLT 22
#define OP_BLTU 54
#define OP_BNE 30
#define OP_BR 6
#define OP_CALL 0
#define OP_CMPEQI 32
#define OP_CMPGEI 8
#define OP_CMPGEUI 40
#define OP_CMPLTI 16
#define OP_CMPLTUI 48
#define OP_CMPNEI 24
#define OP_CUSTOM 50
#define OP_FLUSHD 59
#define OP_FLUSHDA 27
#define OP_INITD 51
#define OP_INITDA 19
#define OP_JMPI 1
#define OP_LDB 7
#define OP_LDBIO 39
#define OP_LDBU 3
#define OP_LDBUIO 35
#define OP_LDH 15
#define OP_LDHIO 47
#define OP_LDHU 11
#define OP_LDHUIO 43
#define OP_LDL 31
#define OP_LDW 23
#define OP_LDWIO 55
#define OP_MULI 36
#define OP_OPX 58
#define OP_ORHI 52
#define OP_ORI 20
#define OP_STB 5
#define OP_STBIO 37
#define OP_STC 29
#define OP_STH 13
#define OP_STHIO 45
#define OP_STW 21
#define OP_STWIO 53
#define OP_XORHI 60
#define OP_XORI 28

/* OPX instruction values */
#define OPX_ADD 49
#define OPX_AND 14
#define OPX_BREAK 52
#define OPX_BRET 9
#define OPX_CALLR 29
#define OPX_CMPEQ 32
#define OPX_CMPGE 8
#define OPX_CMPGEU 40
#define OPX_CMPLT 16
#define OPX_CMPLTU 48
#define OPX_CMPNE 24
#define OPX_CRST 62
#define OPX_DIV 37
#define OPX_DIVU 36
#define OPX_ERET 1
#define OPX_FLUSHI 12
#define OPX_FLUSHP 4
#define OPX_HBREAK 53
#define OPX_INITI 41
#define OPX_INTR 61
#define OPX_JMP 13
#define OPX_MUL 39
#define OPX_MULXSS 31
#define OPX_MULXSU 23
#define OPX_MULXUU 7
#define OPX_NEXTPC 28
#define OPX_NOR 6
#define OPX_OR 22
#define OPX_RDCTL 38
#define OPX_RET 5
#define OPX_ROL 3
#define OPX_ROLI 2
#define OPX_ROR 11
#define OPX_SLL 19
#define OPX_SLLI 18
#define OPX_SRA 59
#define OPX_SRAI 58
#define OPX_SRL 27
#define OPX_SRLI 26
#define OPX_SUB 57
#define OPX_SYNC 54
#define OPX_TRAP 45
#define OPX_WRCTL 46
#define OPX_XOR 30

/* Macros to detect sub-opcode instructions */
#define IS_OPX_INST(Iw) (GET_IW_OP(Iw) == OP_OPX)
#define IS_CUSTOM_INST(Iw) (GET_IW_OP(Iw) == OP_CUSTOM)

/* Instruction property macros */
#define IW_PROP_RESERVED_OP(Iw) (0)

#define IW_PROP_RESERVED_OPX(Iw) (0)

#define IW_PROP_RESERVED(Iw) (0)

#define IW_PROP_SUPERVISOR_ONLY(Iw) ( \
    (op_prop_supervisor_only[GET_IW_OP(Iw)] || \
    (IS_OPX_INST(Iw) && opx_prop_supervisor_only[GET_IW_OPX(Iw)])))

#ifndef ALT_ASM_SRC
extern unsigned char op_prop_supervisor_only[64];
#endif /* ALT_ASM_SRC */

#ifndef ALT_ASM_SRC
extern unsigned char opx_prop_supervisor_only[64];
#endif /* ALT_ASM_SRC */

#define IW_PROP_INITI_FLUSHI(Iw) ( \
  ( \
    ((GET_IW_OPX((Iw)) == OPX_INITI) && IS_OPX_INST(Iw)) || \
    ((GET_IW_OPX((Iw)) == OPX_FLUSHI) && IS_OPX_INST(Iw)) \
  ) \
 \
)

#define IW_PROP_FLUSH_PIPE(Iw) ( \
    (IS_OPX_INST(Iw) && opx_prop_flush_pipe[GET_IW_OPX(Iw)]))

#ifndef ALT_ASM_SRC
extern unsigned char opx_prop_flush_pipe[64];
#endif /* ALT_ASM_SRC */

#define IW_PROP_JMP_INDIRECT_NON_TRAP(Iw) ( \
    (IS_OPX_INST(Iw) && opx_prop_jmp_indirect_non_trap[GET_IW_OPX(Iw)]))

#ifndef ALT_ASM_SRC
extern unsigned char opx_prop_jmp_indirect_non_trap[64];
#endif /* ALT_ASM_SRC */

#define IW_PROP_JMP_INDIRECT(Iw) ( \
    (IS_OPX_INST(Iw) && opx_prop_jmp_indirect[GET_IW_OPX(Iw)]))

#ifndef ALT_ASM_SRC
extern unsigned char opx_prop_jmp_indirect[64];
#endif /* ALT_ASM_SRC */

#define IW_PROP_JMP_DIRECT(Iw) ( \
  ( \
    ((GET_IW_OP((Iw)) == OP_CALL)) || \
    ((GET_IW_OP((Iw)) == OP_JMPI)) \
  ) \
 \
)

#define IW_PROP_MUL_LSW(Iw) ( \
  ( \
    ((GET_IW_OP((Iw)) == OP_MULI)) || \
    ((GET_IW_OPX((Iw)) == OPX_MUL) && IS_OPX_INST(Iw)) \
  ) \
 \
)

#define IW_PROP_MULX(Iw) ( \
    (IS_OPX_INST(Iw) && opx_prop_mulx[GET_IW_OPX(Iw)]))

#ifndef ALT_ASM_SRC
extern unsigned char opx_prop_mulx[64];
#endif /* ALT_ASM_SRC */

#define IW_PROP_MUL(Iw) ( \
    (op_prop_mul[GET_IW_OP(Iw)] || \
    (IS_OPX_INST(Iw) && opx_prop_mul[GET_IW_OPX(Iw)])))

#ifndef ALT_ASM_SRC
extern unsigned char op_prop_mul[64];
#endif /* ALT_ASM_SRC */

#ifndef ALT_ASM_SRC
extern unsigned char opx_prop_mul[64];
#endif /* ALT_ASM_SRC */

#define IW_PROP_DIV_UNSIGNED(Iw) ( \
  ( \
    ((GET_IW_OPX((Iw)) == OPX_DIVU) && IS_OPX_INST(Iw)) \
  ) \
 \
)

#define IW_PROP_DIV_SIGNED(Iw) ( \
  ( \
    ((GET_IW_OPX((Iw)) == OPX_DIV) && IS_OPX_INST(Iw)) \
  ) \
 \
)

#define IW_PROP_DIV(Iw) ( \
  ( \
    ((GET_IW_OPX((Iw)) == OPX_DIVU) && IS_OPX_INST(Iw)) || \
    ((GET_IW_OPX((Iw)) == OPX_DIV) && IS_OPX_INST(Iw)) \
  ) \
 \
)

#define IW_PROP_UNIMPLEMENTED(Iw) (0)

#define IW_PROP_ILLEGAL(Iw) (0)

#define IW_PROP_IMPLICIT_DST_RETADDR(Iw) ( \
  ( \
    ((GET_IW_OP((Iw)) == OP_CALL)) \
  ) \
 \
)

#define IW_PROP_IMPLICIT_DST_ERETADDR(Iw) (0)

#define IW_PROP_INTR(Iw) ( \
  ( \
    ((GET_IW_OPX((Iw)) == OPX_INTR) && IS_OPX_INST(Iw)) \
  ) \
 \
)

#define IW_PROP_EXCEPTION(Iw) ( \
  ( \
    ((GET_IW_OPX((Iw)) == OPX_TRAP) && IS_OPX_INST(Iw)) \
  ) \
 \
)

#define IW_PROP_BREAK(Iw) ( \
  ( \
    ((GET_IW_OPX((Iw)) == OPX_BREAK) && IS_OPX_INST(Iw)) \
  ) \
 \
)

#define IW_PROP_CRST(Iw) (0)

#define IW_PROP_WR_CTL_REG(Iw) ( \
    (IS_OPX_INST(Iw) && opx_prop_wr_ctl_reg[GET_IW_OPX(Iw)]))

#ifndef ALT_ASM_SRC
extern unsigned char opx_prop_wr_ctl_reg[64];
#endif /* ALT_ASM_SRC */

#define IW_PROP_UNCOND_CTI_NON_BR(Iw) ( \
    (op_prop_uncond_cti_non_br[GET_IW_OP(Iw)] || \
    (IS_OPX_INST(Iw) && opx_prop_uncond_cti_non_br[GET_IW_OPX(Iw)])))

#ifndef ALT_ASM_SRC
extern unsigned char op_prop_uncond_cti_non_br[64];
#endif /* ALT_ASM_SRC */

#ifndef ALT_ASM_SRC
extern unsigned char opx_prop_uncond_cti_non_br[64];
#endif /* ALT_ASM_SRC */

#define IW_PROP_RETADDR(Iw) ( \
    (op_prop_retaddr[GET_IW_OP(Iw)] || \
    (IS_OPX_INST(Iw) && opx_prop_retaddr[GET_IW_OPX(Iw)])))

#ifndef ALT_ASM_SRC
extern unsigned char op_prop_retaddr[64];
#endif /* ALT_ASM_SRC */

#ifndef ALT_ASM_SRC
extern unsigned char opx_prop_retaddr[64];
#endif /* ALT_ASM_SRC */

#define IW_PROP_SHIFT_LEFT(Iw) ( \
  ( \
    ((GET_IW_OPX((Iw)) == OPX_SLLI) && IS_OPX_INST(Iw)) || \
    ((GET_IW_OPX((Iw)) == OPX_SLL) && IS_OPX_INST(Iw)) \
  ) \
 \
)

#define IW_PROP_SHIFT_LOGICAL(Iw) ( \
    (IS_OPX_INST(Iw) && opx_prop_shift_logical[GET_IW_OPX(Iw)]))

#ifndef ALT_ASM_SRC
extern unsigned char opx_prop_shift_logical[64];
#endif /* ALT_ASM_SRC */

#define IW_PROP_ROT_LEFT(Iw) ( \
  ( \
    ((GET_IW_OPX((Iw)) == OPX_ROLI) && IS_OPX_INST(Iw)) || \
    ((GET_IW_OPX((Iw)) == OPX_ROL) && IS_OPX_INST(Iw)) \
  ) \
 \
)

#define IW_PROP_SHIFT_ROT_LEFT(Iw) ( \
    (IS_OPX_INST(Iw) && opx_prop_shift_rot_left[GET_IW_OPX(Iw)]))

#ifndef ALT_ASM_SRC
extern unsigned char opx_prop_shift_rot_left[64];
#endif /* ALT_ASM_SRC */

#define IW_PROP_SHIFT_RIGHT_LOGICAL(Iw) ( \
  ( \
    ((GET_IW_OPX((Iw)) == OPX_SRLI) && IS_OPX_INST(Iw)) || \
    ((GET_IW_OPX((Iw)) == OPX_SRL) && IS_OPX_INST(Iw)) \
  ) \
 \
)

#define IW_PROP_SHIFT_RIGHT_ARITH(Iw) ( \
  ( \
    ((GET_IW_OPX((Iw)) == OPX_SRAI) && IS_OPX_INST(Iw)) || \
    ((GET_IW_OPX((Iw)) == OPX_SRA) && IS_OPX_INST(Iw)) \
  ) \
 \
)

#define IW_PROP_SHIFT_RIGHT(Iw) ( \
    (IS_OPX_INST(Iw) && opx_prop_shift_right[GET_IW_OPX(Iw)]))

#ifndef ALT_ASM_SRC
extern unsigned char opx_prop_shift_right[64];
#endif /* ALT_ASM_SRC */

#define IW_PROP_ROT_RIGHT(Iw) ( \
  ( \
    ((GET_IW_OPX((Iw)) == OPX_ROR) && IS_OPX_INST(Iw)) \
  ) \
 \
)

#define IW_PROP_SHIFT_ROT_RIGHT(Iw) ( \
    (IS_OPX_INST(Iw) && opx_prop_shift_rot_right[GET_IW_OPX(Iw)]))

#ifndef ALT_ASM_SRC
extern unsigned char opx_prop_shift_rot_right[64];
#endif /* ALT_ASM_SRC */

#define IW_PROP_SHIFT_ROT(Iw) ( \
    (IS_OPX_INST(Iw) && opx_prop_shift_rot[GET_IW_OPX(Iw)]))

#ifndef ALT_ASM_SRC
extern unsigned char opx_prop_shift_rot[64];
#endif /* ALT_ASM_SRC */

#define IW_PROP_SHIFT_ROT_IMM(Iw) ( \
    (IS_OPX_INST(Iw) && opx_prop_shift_rot_imm[GET_IW_OPX(Iw)]))

#ifndef ALT_ASM_SRC
extern unsigned char opx_prop_shift_rot_imm[64];
#endif /* ALT_ASM_SRC */

#define IW_PROP_ROTATE(Iw) ( \
    (IS_OPX_INST(Iw) && opx_prop_rotate[GET_IW_OPX(Iw)]))

#ifndef ALT_ASM_SRC
extern unsigned char opx_prop_rotate[64];
#endif /* ALT_ASM_SRC */

#define IW_PROP_LOGIC_REG(Iw) ( \
    (IS_OPX_INST(Iw) && opx_prop_logic_reg[GET_IW_OPX(Iw)]))

#ifndef ALT_ASM_SRC
extern unsigned char opx_prop_logic_reg[64];
#endif /* ALT_ASM_SRC */

#define IW_PROP_LOGIC_HI_IMM16(Iw) ( \
    (op_prop_logic_hi_imm16[GET_IW_OP(Iw)]))

#ifndef ALT_ASM_SRC
extern unsigned char op_prop_logic_hi_imm16[64];
#endif /* ALT_ASM_SRC */

#define IW_PROP_LOGIC_LO_IMM16(Iw) ( \
    (op_prop_logic_lo_imm16[GET_IW_OP(Iw)]))

#ifndef ALT_ASM_SRC
extern unsigned char op_prop_logic_lo_imm16[64];
#endif /* ALT_ASM_SRC */

#define IW_PROP_LOGIC_IMM16(Iw) ( \
    (op_prop_logic_imm16[GET_IW_OP(Iw)]))

#ifndef ALT_ASM_SRC
extern unsigned char op_prop_logic_imm16[64];
#endif /* ALT_ASM_SRC */

#define IW_PROP_LOGIC(Iw) ( \
    (op_prop_logic[GET_IW_OP(Iw)] || \
    (IS_OPX_INST(Iw) && opx_prop_logic[GET_IW_OPX(Iw)])))

#ifndef ALT_ASM_SRC
extern unsigned char op_prop_logic[64];
#endif /* ALT_ASM_SRC */

#ifndef ALT_ASM_SRC
extern unsigned char opx_prop_logic[64];
#endif /* ALT_ASM_SRC */

#define IW_PROP_UNSIGNED_LO_IMM16(Iw) ( \
    (op_prop_unsigned_lo_imm16[GET_IW_OP(Iw)] || \
    (IS_OPX_INST(Iw) && opx_prop_unsigned_lo_imm16[GET_IW_OPX(Iw)])))

#ifndef ALT_ASM_SRC
extern unsigned char op_prop_unsigned_lo_imm16[64];
#endif /* ALT_ASM_SRC */

#ifndef ALT_ASM_SRC
extern unsigned char opx_prop_unsigned_lo_imm16[64];
#endif /* ALT_ASM_SRC */

#define IW_PROP_ARITH_IMM16(Iw) ( \
  ( \
    ((GET_IW_OP((Iw)) == OP_ADDI)) || \
    ((GET_IW_OP((Iw)) == OP_MULI)) \
  ) \
 \
)

#define IW_PROP_CMP_IMM16(Iw) ( \
    (op_prop_cmp_imm16[GET_IW_OP(Iw)]))

#ifndef ALT_ASM_SRC
extern unsigned char op_prop_cmp_imm16[64];
#endif /* ALT_ASM_SRC */

#define IW_PROP_JMPI(Iw) ( \
  ( \
    ((GET_IW_OP((Iw)) == OP_JMPI)) \
  ) \
 \
)

#define IW_PROP_CMP_IMM16_WITH_CALL_JMPI(Iw) ( \
    (op_prop_cmp_imm16_with_call_jmpi[GET_IW_OP(Iw)]))

#ifndef ALT_ASM_SRC
extern unsigned char op_prop_cmp_imm16_with_call_jmpi[64];
#endif /* ALT_ASM_SRC */

#define IW_PROP_CMP_REG(Iw) ( \
    (IS_OPX_INST(Iw) && opx_prop_cmp_reg[GET_IW_OPX(Iw)]))

#ifndef ALT_ASM_SRC
extern unsigned char opx_prop_cmp_reg[64];
#endif /* ALT_ASM_SRC */

#define IW_PROP_SRC_IMM5(Iw) ( \
    (IS_OPX_INST(Iw) && opx_prop_src_imm5[GET_IW_OPX(Iw)]))

#ifndef ALT_ASM_SRC
extern unsigned char opx_prop_src_imm5[64];
#endif /* ALT_ASM_SRC */

#define IW_PROP_CMP_WITH_LT(Iw) ( \
    (op_prop_cmp_with_lt[GET_IW_OP(Iw)] || \
    (IS_OPX_INST(Iw) && opx_prop_cmp_with_lt[GET_IW_OPX(Iw)])))

#ifndef ALT_ASM_SRC
extern unsigned char op_prop_cmp_with_lt[64];
#endif /* ALT_ASM_SRC */

#ifndef ALT_ASM_SRC
extern unsigned char opx_prop_cmp_with_lt[64];
#endif /* ALT_ASM_SRC */

#define IW_PROP_CMP_WITH_EQ(Iw) ( \
    (op_prop_cmp_with_eq[GET_IW_OP(Iw)] || \
    (IS_OPX_INST(Iw) && opx_prop_cmp_with_eq[GET_IW_OPX(Iw)])))

#ifndef ALT_ASM_SRC
extern unsigned char op_prop_cmp_with_eq[64];
#endif /* ALT_ASM_SRC */

#ifndef ALT_ASM_SRC
extern unsigned char opx_prop_cmp_with_eq[64];
#endif /* ALT_ASM_SRC */

#define IW_PROP_CMP_WITH_GE(Iw) ( \
    (op_prop_cmp_with_ge[GET_IW_OP(Iw)] || \
    (IS_OPX_INST(Iw) && opx_prop_cmp_with_ge[GET_IW_OPX(Iw)])))

#ifndef ALT_ASM_SRC
extern unsigned char op_prop_cmp_with_ge[64];
#endif /* ALT_ASM_SRC */

#ifndef ALT_ASM_SRC
extern unsigned char opx_prop_cmp_with_ge[64];
#endif /* ALT_ASM_SRC */

#define IW_PROP_CMP_WITH_NE(Iw) ( \
  ( \
    ((GET_IW_OP((Iw)) == OP_CMPNEI)) || \
    ((GET_IW_OPX((Iw)) == OPX_CMPNE) && IS_OPX_INST(Iw)) \
  ) \
 \
)

#define IW_PROP_CMP_ALU_SIGNED(Iw) ( \
    (op_prop_cmp_alu_signed[GET_IW_OP(Iw)] || \
    (IS_OPX_INST(Iw) && opx_prop_cmp_alu_signed[GET_IW_OPX(Iw)])))

#ifndef ALT_ASM_SRC
extern unsigned char op_prop_cmp_alu_signed[64];
#endif /* ALT_ASM_SRC */

#ifndef ALT_ASM_SRC
extern unsigned char opx_prop_cmp_alu_signed[64];
#endif /* ALT_ASM_SRC */

#define IW_PROP_CMP(Iw) ( \
    (op_prop_cmp[GET_IW_OP(Iw)] || \
    (IS_OPX_INST(Iw) && opx_prop_cmp[GET_IW_OPX(Iw)])))

#ifndef ALT_ASM_SRC
extern unsigned char op_prop_cmp[64];
#endif /* ALT_ASM_SRC */

#ifndef ALT_ASM_SRC
extern unsigned char opx_prop_cmp[64];
#endif /* ALT_ASM_SRC */

#define IW_PROP_BR_WITH_LT(Iw) ( \
  ( \
    ((GET_IW_OP((Iw)) == OP_BLT)) || \
    ((GET_IW_OP((Iw)) == OP_BLTU)) \
  ) \
 \
)

#define IW_PROP_BR_WITH_GE(Iw) ( \
  ( \
    ((GET_IW_OP((Iw)) == OP_BGE)) || \
    ((GET_IW_OP((Iw)) == OP_BGEU)) \
  ) \
 \
)

#define IW_PROP_BR_WITH_EQ(Iw) ( \
    (op_prop_br_with_eq[GET_IW_OP(Iw)]))

#ifndef ALT_ASM_SRC
extern unsigned char op_prop_br_with_eq[64];
#endif /* ALT_ASM_SRC */

#define IW_PROP_BR_WITH_NE(Iw) ( \
  ( \
    ((GET_IW_OP((Iw)) == OP_BNE)) \
  ) \
 \
)

#define IW_PROP_BR_ALU_SIGNED(Iw) ( \
  ( \
    ((GET_IW_OP((Iw)) == OP_BGE)) || \
    ((GET_IW_OP((Iw)) == OP_BLT)) \
  ) \
 \
)

#define IW_PROP_BR_COND(Iw) ( \
    (op_prop_br_cond[GET_IW_OP(Iw)]))

#ifndef ALT_ASM_SRC
extern unsigned char op_prop_br_cond[64];
#endif /* ALT_ASM_SRC */

#define IW_PROP_BR_UNCOND(Iw) ( \
  ( \
    ((GET_IW_OP((Iw)) == OP_BR)) \
  ) \
 \
)

#define IW_PROP_BR(Iw) ( \
    (op_prop_br[GET_IW_OP(Iw)]))

#ifndef ALT_ASM_SRC
extern unsigned char op_prop_br[64];
#endif /* ALT_ASM_SRC */

#define IW_PROP_ALU_SUB(Iw) ( \
    (op_prop_alu_sub[GET_IW_OP(Iw)] || \
    (IS_OPX_INST(Iw) && opx_prop_alu_sub[GET_IW_OPX(Iw)])))

#ifndef ALT_ASM_SRC
extern unsigned char op_prop_alu_sub[64];
#endif /* ALT_ASM_SRC */

#ifndef ALT_ASM_SRC
extern unsigned char opx_prop_alu_sub[64];
#endif /* ALT_ASM_SRC */

#define IW_PROP_FORCE_XOR(Iw) ( \
    (op_prop_force_xor[GET_IW_OP(Iw)] || \
    (IS_OPX_INST(Iw) && opx_prop_force_xor[GET_IW_OPX(Iw)])))

#ifndef ALT_ASM_SRC
extern unsigned char op_prop_force_xor[64];
#endif /* ALT_ASM_SRC */

#ifndef ALT_ASM_SRC
extern unsigned char opx_prop_force_xor[64];
#endif /* ALT_ASM_SRC */

#define IW_PROP_LOAD8(Iw) ( \
    (op_prop_load8[GET_IW_OP(Iw)]))

#ifndef ALT_ASM_SRC
extern unsigned char op_prop_load8[64];
#endif /* ALT_ASM_SRC */

#define IW_PROP_LOAD16(Iw) ( \
    (op_prop_load16[GET_IW_OP(Iw)]))

#ifndef ALT_ASM_SRC
extern unsigned char op_prop_load16[64];
#endif /* ALT_ASM_SRC */

#define IW_PROP_LOAD32(Iw) ( \
    (op_prop_load32[GET_IW_OP(Iw)]))

#ifndef ALT_ASM_SRC
extern unsigned char op_prop_load32[64];
#endif /* ALT_ASM_SRC */

#define IW_PROP_LOAD_SIGNED(Iw) ( \
    (op_prop_load_signed[GET_IW_OP(Iw)]))

#ifndef ALT_ASM_SRC
extern unsigned char op_prop_load_signed[64];
#endif /* ALT_ASM_SRC */

#define IW_PROP_LOAD_UNSIGNED(Iw) ( \
    (op_prop_load_unsigned[GET_IW_OP(Iw)]))

#ifndef ALT_ASM_SRC
extern unsigned char op_prop_load_unsigned[64];
#endif /* ALT_ASM_SRC */

#define IW_PROP_LOAD(Iw) ( \
    (op_prop_load[GET_IW_OP(Iw)]))

#ifndef ALT_ASM_SRC
extern unsigned char op_prop_load[64];
#endif /* ALT_ASM_SRC */

#define IW_PROP_DCACHE_MANAGEMENT_NOP(Iw) ( \
    (op_prop_dcache_management_nop[GET_IW_OP(Iw)]))

#ifndef ALT_ASM_SRC
extern unsigned char op_prop_dcache_management_nop[64];
#endif /* ALT_ASM_SRC */

#define IW_PROP_LOAD_DCACHE_MANAGEMENT(Iw) ( \
    (op_prop_load_dcache_management[GET_IW_OP(Iw)]))

#ifndef ALT_ASM_SRC
extern unsigned char op_prop_load_dcache_management[64];
#endif /* ALT_ASM_SRC */

#define IW_PROP_LOAD_NON_IO(Iw) ( \
    (op_prop_load_non_io[GET_IW_OP(Iw)]))

#ifndef ALT_ASM_SRC
extern unsigned char op_prop_load_non_io[64];
#endif /* ALT_ASM_SRC */

#define IW_PROP_STORE8(Iw) ( \
  ( \
    ((GET_IW_OP((Iw)) == OP_STB)) || \
    ((GET_IW_OP((Iw)) == OP_STBIO)) \
  ) \
 \
)

#define IW_PROP_STORE16(Iw) ( \
  ( \
    ((GET_IW_OP((Iw)) == OP_STH)) || \
    ((GET_IW_OP((Iw)) == OP_STHIO)) \
  ) \
 \
)

#define IW_PROP_STORE32(Iw) ( \
    (op_prop_store32[GET_IW_OP(Iw)]))

#ifndef ALT_ASM_SRC
extern unsigned char op_prop_store32[64];
#endif /* ALT_ASM_SRC */

#define IW_PROP_STORE(Iw) ( \
    (op_prop_store[GET_IW_OP(Iw)]))

#ifndef ALT_ASM_SRC
extern unsigned char op_prop_store[64];
#endif /* ALT_ASM_SRC */

#define IW_PROP_STORE_NON_IO(Iw) ( \
    (op_prop_store_non_io[GET_IW_OP(Iw)]))

#ifndef ALT_ASM_SRC
extern unsigned char op_prop_store_non_io[64];
#endif /* ALT_ASM_SRC */

#define IW_PROP_MEM(Iw) ( \
    (op_prop_mem[GET_IW_OP(Iw)]))

#ifndef ALT_ASM_SRC
extern unsigned char op_prop_mem[64];
#endif /* ALT_ASM_SRC */

#define IW_PROP_INITD(Iw) ( \
  ( \
    ((GET_IW_OP((Iw)) == OP_INITD)) \
  ) \
 \
)

#define IW_PROP_INITDA(Iw) ( \
  ( \
    ((GET_IW_OP((Iw)) == OP_INITDA)) \
  ) \
 \
)

#define IW_PROP_FLUSHD(Iw) ( \
  ( \
    ((GET_IW_OP((Iw)) == OP_FLUSHD)) \
  ) \
 \
)

#define IW_PROP_FLUSHDA(Iw) ( \
  ( \
    ((GET_IW_OP((Iw)) == OP_FLUSHDA)) \
  ) \
 \
)

#define IW_PROP_INITD_FLUSHD(Iw) ( \
  ( \
    ((GET_IW_OP((Iw)) == OP_INITD)) || \
    ((GET_IW_OP((Iw)) == OP_FLUSHD)) \
  ) \
 \
)

#define IW_PROP_INITDA_FLUSHDA(Iw) ( \
  ( \
    ((GET_IW_OP((Iw)) == OP_INITDA)) || \
    ((GET_IW_OP((Iw)) == OP_FLUSHDA)) \
  ) \
 \
)

#define IW_PROP_INITD_INITDA(Iw) ( \
  ( \
    ((GET_IW_OP((Iw)) == OP_INITD)) || \
    ((GET_IW_OP((Iw)) == OP_INITDA)) \
  ) \
 \
)

#define IW_PROP_FLUSHD_FLUSHDA(Iw) ( \
  ( \
    ((GET_IW_OP((Iw)) == OP_FLUSHD)) || \
    ((GET_IW_OP((Iw)) == OP_FLUSHDA)) \
  ) \
 \
)

#define IW_PROP_INITD_FLUSHD_FLUSHDA(Iw) ( \
    (op_prop_initd_flushd_flushda[GET_IW_OP(Iw)]))

#ifndef ALT_ASM_SRC
extern unsigned char op_prop_initd_flushd_flushda[64];
#endif /* ALT_ASM_SRC */

#define IW_PROP_DCACHE_MANAGEMENT(Iw) ( \
    (op_prop_dcache_management[GET_IW_OP(Iw)]))

#ifndef ALT_ASM_SRC
extern unsigned char op_prop_dcache_management[64];
#endif /* ALT_ASM_SRC */

#define IW_PROP_LOAD_IO(Iw) ( \
    (op_prop_load_io[GET_IW_OP(Iw)]))

#ifndef ALT_ASM_SRC
extern unsigned char op_prop_load_io[64];
#endif /* ALT_ASM_SRC */

#define IW_PROP_STORE_IO(Iw) ( \
    (op_prop_store_io[GET_IW_OP(Iw)]))

#ifndef ALT_ASM_SRC
extern unsigned char op_prop_store_io[64];
#endif /* ALT_ASM_SRC */

#define IW_PROP_MEM_IO(Iw) ( \
    (op_prop_mem_io[GET_IW_OP(Iw)]))

#ifndef ALT_ASM_SRC
extern unsigned char op_prop_mem_io[64];
#endif /* ALT_ASM_SRC */

#define IW_PROP_ARITH(Iw) ( \
    (op_prop_arith[GET_IW_OP(Iw)] || \
    (IS_OPX_INST(Iw) && opx_prop_arith[GET_IW_OPX(Iw)])))

#ifndef ALT_ASM_SRC
extern unsigned char op_prop_arith[64];
#endif /* ALT_ASM_SRC */

#ifndef ALT_ASM_SRC
extern unsigned char opx_prop_arith[64];
#endif /* ALT_ASM_SRC */

#define IW_PROP_A_NOT_SRC(Iw) ( \
  ( \
    ((GET_IW_OP((Iw)) == OP_CALL)) || \
    ((GET_IW_OP((Iw)) == OP_JMPI)) \
  ) \
  || (IS_CUSTOM_INST(Iw) && !GET_IW_CUSTOM_READRA(Iw)) \
)

#define IW_PROP_B_NOT_SRC(Iw) ( \
    (op_prop_b_not_src[GET_IW_OP(Iw)]) \
  || (IS_CUSTOM_INST(Iw) && !GET_IW_CUSTOM_READRB(Iw)))

#ifndef ALT_ASM_SRC
extern unsigned char op_prop_b_not_src[64];
#endif /* ALT_ASM_SRC */

#define IW_PROP_IGNORE_DST(Iw) ( \
    (op_prop_ignore_dst[GET_IW_OP(Iw)]) \
  || (IS_CUSTOM_INST(Iw) && !GET_IW_CUSTOM_WRITERC(Iw)))

#ifndef ALT_ASM_SRC
extern unsigned char op_prop_ignore_dst[64];
#endif /* ALT_ASM_SRC */

#define IW_PROP_SRC2_CHOOSE_IMM(Iw) ( \
    (op_prop_src2_choose_imm[GET_IW_OP(Iw)] || \
    (IS_OPX_INST(Iw) && opx_prop_src2_choose_imm[GET_IW_OPX(Iw)])))

#ifndef ALT_ASM_SRC
extern unsigned char op_prop_src2_choose_imm[64];
#endif /* ALT_ASM_SRC */

#ifndef ALT_ASM_SRC
extern unsigned char opx_prop_src2_choose_imm[64];
#endif /* ALT_ASM_SRC */

#define IW_PROP_WRCTL_INST(Iw) ( \
  ( \
    ((GET_IW_OPX((Iw)) == OPX_WRCTL) && IS_OPX_INST(Iw)) \
  ) \
 \
)

#define IW_PROP_RDCTL_INST(Iw) ( \
  ( \
    ((GET_IW_OPX((Iw)) == OPX_RDCTL) && IS_OPX_INST(Iw)) \
  ) \
 \
)

#define IW_PROP_MUL_SRC1_SIGNED(Iw) ( \
  ( \
    ((GET_IW_OPX((Iw)) == OPX_MULXSS) && IS_OPX_INST(Iw)) || \
    ((GET_IW_OPX((Iw)) == OPX_MULXSU) && IS_OPX_INST(Iw)) \
  ) \
 \
)

#define IW_PROP_MUL_SRC2_SIGNED(Iw) ( \
  ( \
    ((GET_IW_OPX((Iw)) == OPX_MULXSS) && IS_OPX_INST(Iw)) \
  ) \
 \
)

#define IW_PROP_MUL_SHIFT_SRC1_SIGNED(Iw) ( \
    (IS_OPX_INST(Iw) && opx_prop_mul_shift_src1_signed[GET_IW_OPX(Iw)]))

#ifndef ALT_ASM_SRC
extern unsigned char opx_prop_mul_shift_src1_signed[64];
#endif /* ALT_ASM_SRC */

#define IW_PROP_MUL_SHIFT_SRC2_SIGNED(Iw) ( \
  ( \
    ((GET_IW_OPX((Iw)) == OPX_MULXSS) && IS_OPX_INST(Iw)) \
  ) \
 \
)

#define IW_PROP_DONT_DISPLAY_DST_REG(Iw) ( \
    (op_prop_dont_display_dst_reg[GET_IW_OP(Iw)] || \
    (IS_OPX_INST(Iw) && opx_prop_dont_display_dst_reg[GET_IW_OPX(Iw)])))

#ifndef ALT_ASM_SRC
extern unsigned char op_prop_dont_display_dst_reg[64];
#endif /* ALT_ASM_SRC */

#ifndef ALT_ASM_SRC
extern unsigned char opx_prop_dont_display_dst_reg[64];
#endif /* ALT_ASM_SRC */

#define IW_PROP_DONT_DISPLAY_SRC1_REG(Iw) ( \
    (op_prop_dont_display_src1_reg[GET_IW_OP(Iw)] || \
    (IS_OPX_INST(Iw) && opx_prop_dont_display_src1_reg[GET_IW_OPX(Iw)])))

#ifndef ALT_ASM_SRC
extern unsigned char op_prop_dont_display_src1_reg[64];
#endif /* ALT_ASM_SRC */

#ifndef ALT_ASM_SRC
extern unsigned char opx_prop_dont_display_src1_reg[64];
#endif /* ALT_ASM_SRC */

#define IW_PROP_DONT_DISPLAY_SRC2_REG(Iw) ( \
    (op_prop_dont_display_src2_reg[GET_IW_OP(Iw)] || \
    (IS_OPX_INST(Iw) && opx_prop_dont_display_src2_reg[GET_IW_OPX(Iw)])))

#ifndef ALT_ASM_SRC
extern unsigned char op_prop_dont_display_src2_reg[64];
#endif /* ALT_ASM_SRC */

#ifndef ALT_ASM_SRC
extern unsigned char opx_prop_dont_display_src2_reg[64];
#endif /* ALT_ASM_SRC */

#define IW_PROP_SRC1_NO_X(Iw) ( \
    (op_prop_src1_no_x[GET_IW_OP(Iw)] || \
    (IS_OPX_INST(Iw) && opx_prop_src1_no_x[GET_IW_OPX(Iw)])))

#ifndef ALT_ASM_SRC
extern unsigned char op_prop_src1_no_x[64];
#endif /* ALT_ASM_SRC */

#ifndef ALT_ASM_SRC
extern unsigned char opx_prop_src1_no_x[64];
#endif /* ALT_ASM_SRC */

#define IW_PROP_SRC2_NO_X(Iw) ( \
    (op_prop_src2_no_x[GET_IW_OP(Iw)] || \
    (IS_OPX_INST(Iw) && opx_prop_src2_no_x[GET_IW_OPX(Iw)])))

#ifndef ALT_ASM_SRC
extern unsigned char op_prop_src2_no_x[64];
#endif /* ALT_ASM_SRC */

#ifndef ALT_ASM_SRC
extern unsigned char opx_prop_src2_no_x[64];
#endif /* ALT_ASM_SRC */

/* Instruction types */
#define INST_TYPE_OP  0
#define INST_TYPE_OPX 1

/* Canonical instruction codes independent of encoding */
#define CALL_INST_CODE 0
#define JMPI_INST_CODE 1
#define LDBU_INST_CODE 2
#define ADDI_INST_CODE 3
#define STB_INST_CODE 4
#define BR_INST_CODE 5
#define LDB_INST_CODE 6
#define CMPGEI_INST_CODE 7
#define LDHU_INST_CODE 8
#define ANDI_INST_CODE 9
#define STH_INST_CODE 10
#define BGE_INST_CODE 11
#define LDH_INST_CODE 12
#define CMPLTI_INST_CODE 13
#define INITDA_INST_CODE 14
#define ORI_INST_CODE 15
#define STW_INST_CODE 16
#define BLT_INST_CODE 17
#define LDW_INST_CODE 18
#define CMPNEI_INST_CODE 19
#define FLUSHDA_INST_CODE 20
#define XORI_INST_CODE 21
#define STC_INST_CODE 22
#define BNE_INST_CODE 23
#define LDL_INST_CODE 24
#define CMPEQI_INST_CODE 25
#define LDBUIO_INST_CODE 26
#define MULI_INST_CODE 27
#define STBIO_INST_CODE 28
#define BEQ_INST_CODE 29
#define LDBIO_INST_CODE 30
#define CMPGEUI_INST_CODE 31
#define LDHUIO_INST_CODE 32
#define ANDHI_INST_CODE 33
#define STHIO_INST_CODE 34
#define BGEU_INST_CODE 35
#define LDHIO_INST_CODE 36
#define CMPLTUI_INST_CODE 37
#define CUSTOM_INST_CODE 38
#define INITD_INST_CODE 39
#define ORHI_INST_CODE 40
#define STWIO_INST_CODE 41
#define BLTU_INST_CODE 42
#define LDWIO_INST_CODE 43
#define FLUSHD_INST_CODE 44
#define XORHI_INST_CODE 45
#define ERET_INST_CODE 46
#define ROLI_INST_CODE 47
#define ROL_INST_CODE 48
#define FLUSHP_INST_CODE 49
#define RET_INST_CODE 50
#define NOR_INST_CODE 51
#define MULXUU_INST_CODE 52
#define CMPGE_INST_CODE 53
#define BRET_INST_CODE 54
#define ROR_INST_CODE 55
#define FLUSHI_INST_CODE 56
#define JMP_INST_CODE 57
#define AND_INST_CODE 58
#define CMPLT_INST_CODE 59
#define SLLI_INST_CODE 60
#define SLL_INST_CODE 61
#define OR_INST_CODE 62
#define MULXSU_INST_CODE 63
#define CMPNE_INST_CODE 64
#define SRLI_INST_CODE 65
#define SRL_INST_CODE 66
#define NEXTPC_INST_CODE 67
#define CALLR_INST_CODE 68
#define XOR_INST_CODE 69
#define MULXSS_INST_CODE 70
#define CMPEQ_INST_CODE 71
#define DIVU_INST_CODE 72
#define DIV_INST_CODE 73
#define RDCTL_INST_CODE 74
#define MUL_INST_CODE 75
#define CMPGEU_INST_CODE 76
#define INITI_INST_CODE 77
#define TRAP_INST_CODE 78
#define WRCTL_INST_CODE 79
#define CMPLTU_INST_CODE 80
#define ADD_INST_CODE 81
#define BREAK_INST_CODE 82
#define HBREAK_INST_CODE 83
#define SYNC_INST_CODE 84
#define SUB_INST_CODE 85
#define SRAI_INST_CODE 86
#define SRA_INST_CODE 87
#define INTR_INST_CODE 88
#define CRST_INST_CODE 89
#define RSV_INST_CODE 90
#define NUM_NIOS2_INST_CODES 91

#ifndef ALT_ASM_SRC
/* Instruction information entry */
typedef struct {
     const char *name;     /* Assembly-language instruction name */
     int         instType; /* INST_TYPE_OP or INST_TYPE_OPX */
     unsigned    opcode;   /* Value of instruction word OP/OPX field */
} Nios2InstInfo;

extern Nios2InstInfo nios2InstInfo[NUM_NIOS2_INST_CODES];
#endif /* ALT_ASM_SRC */

/* Returns the instruction code given the 32-bit instruction word */
#define GET_INST_CODE(Iw) \
         (IS_OPX_INST(Iw) ? opxToInstCode[GET_IW_OPX(Iw)] : \
                            opToInstCode[GET_IW_OP(Iw)])

#ifndef ALT_ASM_SRC
extern int opToInstCode[64];
extern int opxToInstCode[64];
#endif /* ALT_ASM_SRC */

/*
 * MMU Memory Region Macros
 */
#define USER_REGION_MIN_VADDR       0x00000000
#define USER_REGION_MAX_VADDR       0x7fffffff
#define KERNEL_MMU_REGION_MIN_VADDR 0x80000000
#define KERNEL_MMU_REGION_MAX_VADDR 0xbfffffff
#define KERNEL_REGION_MIN_VADDR     0xc0000000
#define KERNEL_REGION_MAX_VADDR     0xdfffffff
#define IO_REGION_MIN_VADDR         0xe0000000
#define IO_REGION_MAX_VADDR         0xffffffff

#define MMU_PAGE_SIZE (0x1 << (MMU_ADDR_PAGE_OFFSET_SZ))

#define isMmuUserRegion(Vaddr)          \
    (GET_MMU_ADDR_USER_REGION(Vaddr) == MMU_ADDR_USER_REGION)
#define isMmuKernelMmuRegion(Vaddr)     \
    (GET_MMU_ADDR_KERNEL_MMU_REGION(Vaddr) == MMU_ADDR_KERNEL_MMU_REGION)
#define isMmuKernelRegion(Vaddr)        \
    (GET_MMU_ADDR_KERNEL_REGION(Vaddr) == MMU_ADDR_KERNEL_REGION)
#define isMmuIORegion(Vaddr)            \
    (GET_MMU_ADDR_IO_REGION(Vaddr) == MMU_ADDR_IO_REGION)

/* Does this virtual address bypass the TLB? */
#define vaddrBypassTlb(Vaddr)                \
    (GET_MMU_ADDR_BYPASS_TLB(Vaddr) == MMU_ADDR_BYPASS_TLB)

/* If TLB is bypassed, is the address cacheable or uncachable. */
#define vaddrBypassTlbCacheable(Vaddr)       \
    (GET_MMU_ADDR_BYPASS_TLB_CACHEABLE(Vaddr) == MMU_ADDR_BYPASS_TLB_CACHEABLE)

/*
 * Compute physical address for regions that bypass the TLB.
 * Just need to clear some top bits.
 */
#define bypassTlbVaddrToPaddr(Vaddr)    \
    ((Vaddr) & (MMU_ADDR_BYPASS_TLB_PADDR_MASK << \
                MMU_ADDR_BYPASS_TLB_PADDR_LSB))

/*
 * Will the physical address fit in the Kernel/IO region virtual address space?
 */
#define fitsInKernelRegion(Paddr)       \
    (GET_MMU_ADDR_KERNEL_REGION(Paddr) == 0)
#define fitsInIORegion(Paddr)           \
    (GET_MMU_ADDR_IO_REGION(Paddr) == 0)

/* Convert a physical address to a Kernel/IO region virtual address. */
#define paddrToKernelRegionVaddr(Paddr) \
    ((Paddr) | (MMU_ADDR_KERNEL_REGION << MMU_ADDR_KERNEL_REGION_LSB))
#define paddrToIORegionVaddr(Paddr)     \
    ((Paddr) | (MMU_ADDR_IO_REGION << MMU_ADDR_IO_REGION_LSB))

/*
 * Convert a virtual address to a Kernel/IO region virtual address.
 * Uses bypassTlbVaddrToPaddr to clear top bits.
 */
#define vaddrToKernelRegionVaddr(Vaddr) \
    paddrToKernelRegionVaddr(bypassTlbVaddrToPaddr(Vaddr))
#define vaddrToIORegionVaddr(Vaddr) \
    paddrToIORegionVaddr(bypassTlbVaddrToPaddr(Vaddr))

/* Convert between VPN/PFN and virtual/physical addresses. */
#define vpnToVaddr(Vpn) ((Vpn) << MMU_ADDR_VPN_LSB)
#define pfnToPaddr(Pfn) ((Pfn) << MMU_ADDR_PFN_LSB)
#define vaddrToVpn(Vaddr) GET_MMU_ADDR_VPN(Vaddr)
#define paddrToPfn(Paddr) GET_MMU_ADDR_PFN(Paddr)

/* Bitwise OR with a KERNEL region address to make it an IO region address */
#define KERNEL_TO_IO_REGION 0x20000000

/* Exception information */
#ifndef ALT_ASM_SRC
typedef struct {
    const char *name;
    int priority;
    int subPriority; /* -1 if none */
    int causeId; /* -1 if none */
    int recordAddr;
} ExcInfo;

extern ExcInfo excInfo[NUM_EXC_IDS];
#endif /* ALT_ASM_SRC */

#endif /* _NIOS2_ISA_H_ */
/* nios2-opc.c -- Altera New Jersey opcode list.

   Copyright (C) 2003
   by Nigel Gray (ngray@altera.com).

This file is part of GDB, GAS, and the GNU binutils.

GDB, GAS, and the GNU binutils are free software; you can redistribute
them and/or modify them under the terms of the GNU General Public
License as published by the Free Software Foundation; either version
1, or (at your option) any later version.

GDB, GAS, and the GNU binutils are distributed in the hope that they
will be useful, but WITHOUT ANY WARRANTY; without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this file; see the file COPYING.  If not,
see <http://www.gnu.org/licenses/>.  */

#include <stdio.h>
/*#include "nios2.h" */

/* Register string table */

const struct nios2_reg nios2_builtin_regs[] = {
  {"zero", 0},
  {"at", 1},  /* assembler temporary */
  {"r2", 2},
  {"r3", 3},
  {"r4", 4},
  {"r5", 5},
  {"r6", 6},
  {"r7", 7},
  {"r8", 8},
  {"r9", 9},
  {"r10", 10},
  {"r11", 11},
  {"r12", 12},
  {"r13", 13},
  {"r14", 14},
  {"r15", 15},
  {"r16", 16},
  {"r17", 17},
  {"r18", 18},
  {"r19", 19},
  {"r20", 20},
  {"r21", 21},
  {"r22", 22},
  {"r23", 23},
  {"et", 24},
  {"bt", 25},
  {"gp", 26},  /* global pointer */
  {"sp", 27},  /* stack pointer */
  {"fp", 28},  /* frame pointer */
  {"ea", 29},  /* exception return address */
  {"ba", 30},  /* breakpoint return address */
  {"ra", 31},  /* return address */

  /* alternative names for special registers */
  {"r0", 0},
  {"r1", 1},
  {"r24", 24},
  {"r25", 25},
  {"r26", 26},
  {"r27", 27},
  {"r28", 28},
  {"r29", 29},
  {"r30", 30},
  {"r31", 31},

  /* control register names */
  {"status", 0},
  {"estatus", 1},
  {"bstatus", 2},
  {"ienable", 3},
  {"ipending", 4},
  {"cpuid", 5},
  {"ctl6", 6},
  {"exception", 7},
  {"pteaddr", 8},
  {"tlbacc", 9},
  {"tlbmisc", 10},
  {"fstatus", 11},
  {"badaddr", 12},
  {"config", 13},
  {"mpubase", 14},
  {"mpuacc", 15},
  {"ctl16", 16},
  {"ctl17", 17},
  {"ctl18", 18},
  {"ctl19", 19},
  {"ctl20", 20},
  {"ctl21", 21},
  {"ctl22", 22},
  {"ctl23", 23},
  {"ctl24", 24},
  {"ctl25", 25},
  {"ctl26", 26},
  {"ctl27", 27},
  {"ctl28", 28},
  {"ctl29", 29},
  {"ctl30", 30},
  {"ctl31", 31},

  /* alternative names for special control registers */
  {"ctl0", 0},
  {"ctl1", 1},
  {"ctl2", 2},
  {"ctl3", 3},
  {"ctl4", 4},
  {"ctl5", 5},
  {"ctl7", 7},
  {"ctl8", 8},
  {"ctl9", 9},
  {"ctl10", 10},
  {"ctl11", 11},
  {"ctl12", 12},
  {"ctl13", 13},
  {"ctl14", 14},
  {"ctl15", 15},


  /* coprocessor register names */
  {"c0", 0},
  {"c1", 1},
  {"c2", 2},
  {"c3", 3},
  {"c4", 4},
  {"c5", 5},
  {"c6", 6},
  {"c7", 7},
  {"c8", 8},
  {"c9", 9},
  {"c10", 10},
  {"c11", 11},
  {"c12", 12},
  {"c13", 13},
  {"c14", 14},
  {"c15", 15},
  {"c16", 16},
  {"c17", 17},
  {"c18", 18},
  {"c19", 19},
  {"c20", 20},
  {"c21", 21},
  {"c22", 22},
  {"c23", 23},
  {"c24", 24},
  {"c25", 25},
  {"c26", 26},
  {"c27", 27},
  {"c28", 28},
  {"c29", 29},
  {"c30", 30},
  {"c31", 31},
};

#define NIOS2_NUM_REGS \
       ((sizeof(nios2_builtin_regs)) / (sizeof(nios2_builtin_regs[0])))
const int nios2_num_builtin_regs = NIOS2_NUM_REGS;

/* const removed from the following to allow for dynamic extensions to the
 * built-in instruction set. */
struct nios2_reg *nios2_regs = (struct nios2_reg *) nios2_builtin_regs;
int nios2_num_regs = NIOS2_NUM_REGS;
#undef NIOS2_NUM_REGS

/* overflow message string templates */

const char *overflow_msgs[] = {
    "call target address 0x%08x out of range 0x%08x to 0x%08x",
    "branch offset %d out of range %d to %d",
    "%s offset %d out of range %d to %d",
    "immediate value %d out of range %d to %d",
    "immediate value %u out of range %u to %u",
    "immediate value %u out of range %u to %u",
    "custom instruction opcode %u out of range %u to %u",
};



/*------------------------------------------------------------------------------
   This is the opcode table used by the New Jersey GNU as, disassembler and GDB
  ----------------------------------------------------------------------------*/

/*
       The following letters can appear in the args field of the nios2_opcode
       structure:

       c - a 5-bit control register index or break opcode
       d - a 5-bit destination register index
       s - a 5-bit left source register index
       t - a 5-bit right source register index
       i - a 16-bit signed immediate
       u - a 16-bit unsigned immediate

       j - a 5-bit unsigned immediate
       k - a 6-bit unsigned immediate
       l - an 8-bit unsigned immediate
       m - a 26-bit unsigned immediate
*/

/* *INDENT-OFF* */
/* FIXME: Re-format for GNU standards */
const struct nios2_opcode nios2_builtin_opcodes[] = {
    /* name, args, args_test, num_args, match, mask, pinfo */
    {"add", "d,s,t", "d,s,t,E", 3, OP_MATCH_ADD,
        OP_MASK_ROPX | OP_MASK_ROP, 0,
        no_overflow },
    {"addi", "t,s,i", "t,s,i,E", 3, OP_MATCH_ADDI,
        OP_MASK_IOP, NIOS2_INSN_ADDI,
        signed_immed16_overflow },
    {"subi", "t,s,i", "t,s,i,E", 3, OP_MATCH_ADDI,
        OP_MASK_IOP, NIOS2_INSN_MACRO,
        signed_immed16_overflow },
    {"and", "d,s,t", "d,s,t,E", 3, OP_MATCH_AND,
        OP_MASK_ROPX | OP_MASK_ROP, 0,
        no_overflow },
    {"andhi", "t,s,u", "t,s,u,E", 3, OP_MATCH_ANDHI,
        OP_MASK_IOP, 0,
        unsigned_immed16_overflow },
    {"andi", "t,s,u", "t,s,u,E", 3, OP_MATCH_ANDI,
        OP_MASK_IOP, NIOS2_INSN_ANDI,
        unsigned_immed16_overflow },
    {"beq", "s,t,o", "s,t,o,E", 3, OP_MATCH_BEQ,
        OP_MASK_IOP, NIOS2_INSN_CBRANCH,
        branch_target_overflow },
    {"bge", "s,t,o", "s,t,o,E", 3, OP_MATCH_BGE,
        OP_MASK_IOP, NIOS2_INSN_CBRANCH,
        branch_target_overflow },
    {"bgeu", "s,t,o", "s,t,o,E", 3, OP_MATCH_BGEU,
        OP_MASK_IOP, NIOS2_INSN_CBRANCH,
        branch_target_overflow },
    {"bgt", "s,t,o", "s,t,o,E", 3, OP_MATCH_BLT,
        OP_MASK_IOP, NIOS2_INSN_MACRO|NIOS2_INSN_CBRANCH,
        branch_target_overflow },
    {"bgtu", "s,t,o", "s,t,o,E", 3, OP_MATCH_BLTU,
        OP_MASK_IOP, NIOS2_INSN_MACRO|NIOS2_INSN_CBRANCH,
        branch_target_overflow },
    {"ble", "s,t,o", "s,t,o,E", 3, OP_MATCH_BGE,
        OP_MASK_IOP, NIOS2_INSN_MACRO|NIOS2_INSN_CBRANCH,
        branch_target_overflow },
    {"bleu", "s,t,o", "s,t,o,E", 3, OP_MATCH_BGEU,
        OP_MASK_IOP, NIOS2_INSN_MACRO|NIOS2_INSN_CBRANCH,
        branch_target_overflow },
    {"blt", "s,t,o", "s,t,o,E", 3, OP_MATCH_BLT,
        OP_MASK_IOP, NIOS2_INSN_CBRANCH,
        branch_target_overflow },
    {"bltu", "s,t,o", "s,t,o,E", 3, OP_MATCH_BLTU,
        OP_MASK_IOP, NIOS2_INSN_CBRANCH,
        branch_target_overflow },
    {"bne", "s,t,o", "s,t,o,E", 3, OP_MATCH_BNE,
        OP_MASK_IOP, NIOS2_INSN_CBRANCH,
        branch_target_overflow },
    {"br", "o", "o,E", 1, OP_MATCH_BR,
        OP_MASK_IOP, NIOS2_INSN_UBRANCH,
        branch_target_overflow },
    {"break", "b", "b,E", 1, OP_MATCH_BREAK,
        OP_MASK_BREAK, 0,
        no_overflow },
    {"bret", "", "E", 0, OP_MATCH_BRET,
        OP_MASK, 0,
        no_overflow },
    {"flushd", "i(s)", "i(s)E", 2, OP_MATCH_FLUSHD,
        OP_MASK_IOP, 0,
        signed_immed16_overflow },
    {"flushda", "i(s)", "i(s)E", 2, OP_MATCH_FLUSHDA,
        OP_MASK_IOP, 0,
        signed_immed16_overflow },
    {"flushi", "s", "s,E", 1, OP_MATCH_FLUSHI,
        OP_MASK_FLUSHI, 0,
        no_overflow },
    {"flushp", "", "E", 0, OP_MATCH_FLUSHP,
        OP_MASK, 0,
        no_overflow },
    {"initd", "i(s)", "i(s)E", 2, OP_MATCH_INITD,
        OP_MASK_IOP, 0,
        signed_immed16_overflow },
    {"initda", "i(s)", "i(s)E", 2, OP_MATCH_INITDA,
        OP_MASK_IOP, 0,
        signed_immed16_overflow },
    {"initi", "s", "s,E", 1, OP_MATCH_INITI,
        OP_MASK_INITI, 0,
        no_overflow },
    {"call", "m", "m,E", 1, OP_MATCH_CALL,
        OP_MASK_IOP, NIOS2_INSN_CALL,
        call_target_overflow },
    {"callr", "s", "s,E", 1, OP_MATCH_CALLR,
        OP_MASK_CALLR, 0,
        no_overflow },
    {"cmpeq", "d,s,t", "d,s,t,E", 3, OP_MATCH_CMPEQ,
        OP_MASK_ROPX | OP_MASK_ROP, 0,
        no_overflow },
    {"cmpeqi", "t,s,i", "t,s,i,E", 3, OP_MATCH_CMPEQI,
        OP_MASK_IOP, 0,
        signed_immed16_overflow },
    {"cmpge", "d,s,t", "d,s,t,E", 3, OP_MATCH_CMPGE,
        OP_MASK_ROPX | OP_MASK_ROP, 0,
        no_overflow },
    {"cmpgei", "t,s,i", "t,s,i,E", 3, OP_MATCH_CMPGEI,
        OP_MASK_IOP, 0,
        signed_immed16_overflow },
    {"cmpgeu", "d,s,t", "d,s,t,E", 3, OP_MATCH_CMPGEU,
        OP_MASK_ROPX | OP_MASK_ROP, 0,
        no_overflow },
    {"cmpgeui", "t,s,u", "t,s,u,E", 3, OP_MATCH_CMPGEUI,
        OP_MASK_IOP, 0,
        unsigned_immed16_overflow },
    {"cmpgt", "d,s,t", "d,s,t,E", 3, OP_MATCH_CMPLT,
        OP_MASK_ROPX | OP_MASK_ROP, NIOS2_INSN_MACRO,
        no_overflow },
    {"cmpgti", "t,s,i", "t,s,i,E", 3, OP_MATCH_CMPGEI,
        OP_MASK_IOP, NIOS2_INSN_MACRO,
        signed_immed16_overflow },
    {"cmpgtu", "d,s,t", "d,s,t,E", 3, OP_MATCH_CMPLTU,
        OP_MASK_ROPX | OP_MASK_ROP, NIOS2_INSN_MACRO,
        no_overflow },
    {"cmpgtui", "t,s,u", "t,s,u,E", 3, OP_MATCH_CMPGEUI,
        OP_MASK_IOP, NIOS2_INSN_MACRO,
        unsigned_immed16_overflow },
    {"cmple", "d,s,t", "d,s,t,E", 3, OP_MATCH_CMPGE,
        OP_MASK_ROPX | OP_MASK_ROP, NIOS2_INSN_MACRO,
        no_overflow },
    {"cmplei", "t,s,i", "t,s,i,E", 3, OP_MATCH_CMPLTI,
        OP_MASK_IOP, NIOS2_INSN_MACRO,
        signed_immed16_overflow },
    {"cmpleu", "d,s,t", "d,s,t,E", 3, OP_MATCH_CMPGEU,
        OP_MASK_ROPX | OP_MASK_ROP, NIOS2_INSN_MACRO,
        no_overflow },
    {"cmpleui", "t,s,u", "t,s,u,E", 3, OP_MATCH_CMPLTUI,
        OP_MASK_IOP, NIOS2_INSN_MACRO,
        unsigned_immed16_overflow },
    {"cmplt", "d,s,t", "d,s,t,E", 3, OP_MATCH_CMPLT,
        OP_MASK_ROPX | OP_MASK_ROP, 0,
        no_overflow },
    {"cmplti", "t,s,i", "t,s,i,E", 3, OP_MATCH_CMPLTI,
        OP_MASK_IOP, 0,
        signed_immed16_overflow },
    {"cmpltu", "d,s,t", "d,s,t,E", 3, OP_MATCH_CMPLTU,
        OP_MASK_ROPX | OP_MASK_ROP, 0,
        no_overflow },
    {"cmpltui", "t,s,u", "t,s,u,E", 3, OP_MATCH_CMPLTUI,
        OP_MASK_IOP, 0,
        unsigned_immed16_overflow },
    {"cmpne", "d,s,t", "d,s,t,E", 3, OP_MATCH_CMPNE,
        OP_MASK_ROPX | OP_MASK_ROP, 0,
        no_overflow },
    {"cmpnei", "t,s,i", "t,s,i,E", 3, OP_MATCH_CMPNEI,
        OP_MASK_IOP, 0,
        signed_immed16_overflow },
    {"div", "d,s,t", "d,s,t,E", 3, OP_MATCH_DIV,
        OP_MASK_ROPX | OP_MASK_ROP, 0,
        no_overflow },
    {"divu", "d,s,t", "d,s,t,E", 3, OP_MATCH_DIVU,
        OP_MASK_ROPX | OP_MASK_ROP, 0,
        no_overflow },
    {"jmp", "s", "s,E", 1, OP_MATCH_JMP,
        OP_MASK_JMP, 0,
        no_overflow },
    {"jmpi", "m", "m,E", 1, OP_MATCH_JMPI,
        OP_MASK_IOP, 0,
        no_overflow },
    {"ldb", "t,i(s)", "t,i(s)E", 3, OP_MATCH_LDB,
        OP_MASK_IOP, 0,
        address_offset_overflow },
    {"ldbio", "t,i(s)", "t,i(s)E", 3, OP_MATCH_LDBIO,
        OP_MASK_IOP, 0,
        address_offset_overflow },
    {"ldbu", "t,i(s)", "t,i(s)E", 3, OP_MATCH_LDBU,
        OP_MASK_IOP, 0,
        address_offset_overflow },
    {"ldbuio", "t,i(s)", "t,i(s)E", 3, OP_MATCH_LDBUIO,
        OP_MASK_IOP, 0,
        address_offset_overflow },
    {"ldh", "t,i(s)", "t,i(s)E", 3, OP_MATCH_LDH,
        OP_MASK_IOP, 0,
        address_offset_overflow },
    {"ldhio", "t,i(s)", "t,i(s)E", 3, OP_MATCH_LDHIO,
        OP_MASK_IOP, 0,
        address_offset_overflow },
    {"ldhu", "t,i(s)", "t,i(s)E", 3, OP_MATCH_LDHU,
        OP_MASK_IOP, 0,
        address_offset_overflow },
    {"ldhuio", "t,i(s)", "t,i(s)E", 3, OP_MATCH_LDHUIO,
        OP_MASK_IOP, 0,
        address_offset_overflow },
    {"ldl", "t,i(s)", "t,i(s)E", 3, OP_MATCH_LDL,
        OP_MASK_IOP, 0,
        address_offset_overflow },
    {"ldw", "t,i(s)", "t,i(s)E", 3, OP_MATCH_LDW,
        OP_MASK_IOP, 0,
        address_offset_overflow },
    {"ldwio", "t,i(s)", "t,i(s)E", 3, OP_MATCH_LDWIO,
        OP_MASK_IOP, 0,
        address_offset_overflow },
    {"mov", "d,s", "d,s,E", 2, OP_MATCH_ADD,
        OP_MASK_RRT|OP_MASK_ROPX|OP_MASK_ROP, NIOS2_INSN_MACRO_MOV,
        no_overflow },
    {"movhi", "t,u", "t,u,E", 2, OP_MATCH_ORHI,
        OP_MASK_IRS|OP_MASK_IOP, NIOS2_INSN_MACRO_MOVI,
        unsigned_immed16_overflow },
    {"movui", "t,u", "t,u,E", 2, OP_MATCH_ORI,
        OP_MASK_IRS|OP_MASK_IOP, NIOS2_INSN_MACRO_MOVI,
        unsigned_immed16_overflow },
    {"movi", "t,i", "t,i,E", 2, OP_MATCH_ADDI,
        OP_MASK_IRS|OP_MASK_IOP, NIOS2_INSN_MACRO_MOVI,
        signed_immed16_overflow },
    /* movia expands to two instructions so there is no mask or match */
    {"movia", "t,o", "t,o,E", 2, OP_MATCH_ORHI,
        OP_MASK_IOP, NIOS2_INSN_MACRO_MOVIA,
        no_overflow },
    {"mul", "d,s,t", "d,s,t,E", 3, OP_MATCH_MUL,
        OP_MASK_ROPX | OP_MASK_ROP, 0,
        no_overflow },
    {"muli", "t,s,i", "t,s,i,E", 3, OP_MATCH_MULI,
        OP_MASK_IOP, 0,
        signed_immed16_overflow },
    {"mulxss", "d,s,t", "d,s,t,E", 3, OP_MATCH_MULXSS,
        OP_MASK_ROPX | OP_MASK_ROP, 0,
        no_overflow },
    {"mulxsu", "d,s,t", "d,s,t,E", 3, OP_MATCH_MULXSU,
        OP_MASK_ROPX | OP_MASK_ROP, 0,
        no_overflow },
    {"mulxuu", "d,s,t", "d,s,t,E", 3, OP_MATCH_MULXUU,
        OP_MASK_ROPX | OP_MASK_ROP, 0,
        no_overflow },
    {"nextpc", "d", "d,E", 1, OP_MATCH_NEXTPC,
        OP_MASK_NEXTPC, 0,
        no_overflow },
    {"nop", "", "E", 0, OP_MATCH_ADD,
        OP_MASK, NIOS2_INSN_MACRO_MOV,
        no_overflow },
    {"nor", "d,s,t", "d,s,t,E", 3, OP_MATCH_NOR,
        OP_MASK_ROPX | OP_MASK_ROP, 0,
        no_overflow },
    {"or", "d,s,t", "d,s,t,E", 3, OP_MATCH_OR,
        OP_MASK_ROPX | OP_MASK_ROP, 0,
        no_overflow },
    {"orhi", "t,s,u", "t,s,u,E", 3, OP_MATCH_ORHI,
        OP_MASK_IOP, 0,
        unsigned_immed16_overflow },
    {"ori", "t,s,u", "t,s,u,E", 3, OP_MATCH_ORI,
        OP_MASK_IOP, NIOS2_INSN_ORI,
        unsigned_immed16_overflow },
    {"rdctl", "d,c", "d,c,E", 2, OP_MATCH_RDCTL,
        OP_MASK_RDCTL, 0,
        no_overflow },
    {"ret", "", "E", 0, OP_MATCH_RET,
        OP_MASK, 0,
        no_overflow },
    {"rol", "d,s,t", "d,s,t,E", 3, OP_MATCH_ROL,
        OP_MASK_ROPX | OP_MASK_ROP, 0,
        no_overflow },
    {"roli", "d,s,j", "d,s,j,E", 3, OP_MATCH_ROLI,
        OP_MASK_ROLI, 0,
        unsigned_immed5_overflow },
    {"ror", "d,s,t", "d,s,t,E", 3, OP_MATCH_ROR,
        OP_MASK_ROPX | OP_MASK_ROP, 0,
        no_overflow },
    {"sll", "d,s,t", "d,s,t,E", 3, OP_MATCH_SLL,
        OP_MASK_ROPX | OP_MASK_ROP, 0,
        no_overflow },
    {"slli", "d,s,j", "d,s,j,E", 3, OP_MATCH_SLLI,
        OP_MASK_SLLI, 0,
        unsigned_immed5_overflow },
    {"sra", "d,s,t", "d,s,t,E", 3, OP_MATCH_SRA,
        OP_MASK_ROPX | OP_MASK_ROP, 0,
        no_overflow },
    {"srai", "d,s,j", "d,s,j,E", 3, OP_MATCH_SRAI,
        OP_MASK_SRAI, 0,
        unsigned_immed5_overflow },
    {"srl", "d,s,t", "d,s,t,E", 3, OP_MATCH_SRL,
        OP_MASK_ROPX | OP_MASK_ROP, 0,
        no_overflow },
    {"srli", "d,s,j", "d,s,j,E", 3, OP_MATCH_SRLI,
        OP_MASK_SRLI, 0,
        unsigned_immed5_overflow },
    {"stb", "t,i(s)", "t,i(s)E", 3, OP_MATCH_STB,
        OP_MASK_IOP, 0,
        address_offset_overflow },
    {"stbio", "t,i(s)", "t,i(s)E", 3, OP_MATCH_STBIO,
        OP_MASK_IOP, 0,
        address_offset_overflow },
    {"stc", "t,i(s)", "t,i(s)E", 3, OP_MATCH_STC,
        OP_MASK_IOP, 0,
        address_offset_overflow },
    {"sth", "t,i(s)", "t,i(s)E", 3, OP_MATCH_STH,
        OP_MASK_IOP, 0,
        address_offset_overflow },
    {"sthio", "t,i(s)", "t,i(s)E", 3, OP_MATCH_STHIO,
        OP_MASK_IOP, 0,
        address_offset_overflow },
    {"stw", "t,i(s)", "t,i(s)E", 3, OP_MATCH_STW,
        OP_MASK_IOP, 0,
        address_offset_overflow },
    {"stwio", "t,i(s)", "t,i(s)E", 3, OP_MATCH_STWIO,
        OP_MASK_IOP, 0,
        address_offset_overflow },
    {"sub", "d,s,t", "d,s,t,E", 3, OP_MATCH_SUB,
        OP_MASK_ROPX | OP_MASK_ROP, 0,
        no_overflow },
    {"sync", "", "E", 0, OP_MATCH_SYNC,
        OP_MASK_SYNC, 0,
        no_overflow },
    {"trap", "", "E", 0, OP_MATCH_TRAP,
        OP_MASK_TRAP, 0,
        no_overflow },
    {"eret", "", "E", 0, OP_MATCH_ERET,
        OP_MASK, 0,
        no_overflow },
    {"custom", "l,d,s,t", "l,d,s,t,E", 4, OP_MATCH_CUSTOM,
        OP_MASK_ROP, 0,
        custom_opcode_overflow },
    {"wrctl", "c,s", "c,s,E", 2, OP_MATCH_WRCTL,
        OP_MASK_WRCTL, 0,
        no_overflow },
    {"xor", "d,s,t", "d,s,t,E", 3, OP_MATCH_XOR,
        OP_MASK_ROPX | OP_MASK_ROP, 0,
        no_overflow },
    {"xorhi", "t,s,u", "t,s,u,E", 3, OP_MATCH_XORHI,
        OP_MASK_IOP, 0,
        unsigned_immed16_overflow },
    {"xori", "t,s,u", "t,s,u,E", 3, OP_MATCH_XORI,
        OP_MASK_IOP, NIOS2_INSN_XORI,
        unsigned_immed16_overflow }
};
/* *INDENT-ON* */

#define NIOS2_NUM_OPCODES \
       ((sizeof(nios2_builtin_opcodes)) / (sizeof(nios2_builtin_opcodes[0])))
const int bfd_nios2_num_builtin_opcodes = NIOS2_NUM_OPCODES;

/* const removed from the following to allow for dynamic extensions to the
 * built-in instruction set. */
struct nios2_opcode *nios2_opcodes =
  (struct nios2_opcode *) nios2_builtin_opcodes;
int bfd_nios2_num_opcodes = NIOS2_NUM_OPCODES;
#undef NIOS2_NUM_OPCODES
/* nios2-dis.c -- Altera New Jersey disassemble routines.

   Copyright (C) 2003
   by Nigel Gray (ngray@altera.com).

This file is part of GDB, GAS, and the GNU binutils.

GDB, GAS, and the GNU binutils are free software; you can redistribute
them and/or modify them under the terms of the GNU General Public
License as published by the Free Software Foundation; either version
1, or (at your option) any later version.

GDB, GAS, and the GNU binutils are distributed in the hope that they
will be useful, but WITHOUT ANY WARRANTY; without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this file; see the file COPYING.  If not,
see <http://www.gnu.org/licenses/>.  */

#include <stdlib.h>
#include <assert.h>
#include <string.h>
/*#include "dis-asm.h"*/
/*#include "nios2.h"*/

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

/* length of New Jersey instruction in bytes */
#define INSNLEN 4

/* helper function prototypes */
static int nios2_disassemble(bfd_vma, unsigned long, disassemble_info *);
static void nios2_init_opcode_hash(void);


static int nios2_print_insn_arg(const char *argptr, unsigned long opcode,
                                bfd_vma address, disassemble_info *info);


/* print_insn_nios2 is the main disassemble function for New Jersey.
   The function diassembler(abfd) (source in disassemble.c) returns a
   pointer to this either print_insn_big_nios2 or
   print_insn_little_nios2, which in turn call this function, when the
   bfd machine type is New Jersey. print_insn_nios2 reads the
   instruction word at the address given, and prints the disassembled
   instruction on the stream info->stream using info->fprintf_func. */

int
print_insn_nios2(bfd_vma address, disassemble_info *info)
{
    /* buffer into which the instruction bytes are written */
    bfd_byte buffer[INSNLEN];
    /* used to indicate return status from function calls */
    int status;

    assert(info != NULL);

    status = (*info->read_memory_func) (address, buffer, INSNLEN, info);
    if (status == 0) {
        unsigned long insn;
        insn = (unsigned long) bfd_getl32(buffer);
        status = nios2_disassemble(address, insn, info);
    } else {
        (*info->memory_error_func) (status, address, info);
        status = -1;
    }
    return status;
}

/* Data structures used by the opcode hash table */

typedef struct _nios2_opcode_hash {
    const struct nios2_opcode *opcode;
    struct _nios2_opcode_hash *next;
} nios2_opcode_hash;

static bfd_boolean nios2_hash_init;
static nios2_opcode_hash *nios2_hash[(OP_MASK_OP) + 1];

/* separate hash table for pseudo-ops */
static nios2_opcode_hash *nios2_ps_hash[(OP_MASK_OP) + 1];

/* Function to initialize the opcode hash table */

void
nios2_init_opcode_hash(void)
{
    unsigned int i;
    register const struct nios2_opcode *op;
    nios2_opcode_hash *tmp_hash;

    for (i = 0; i <= OP_MASK_OP; ++i) {
        nios2_hash[0] = NULL;
    }
    for (i = 0; i <= OP_MASK_OP; i++) {
        for (op = nios2_opcodes; op < &nios2_opcodes[NUMOPCODES]; op++) {
            if ((op->pinfo & NIOS2_INSN_MACRO) == NIOS2_INSN_MACRO) {
                if (i == ((op->match >> OP_SH_OP) & OP_MASK_OP) &&
                    (op->pinfo &
                     (NIOS2_INSN_MACRO_MOV | NIOS2_INSN_MACRO_MOVI) &
                     0x7fffffff) != 0) {
                    tmp_hash = nios2_ps_hash[i];
                    if (tmp_hash == NULL) {
                        tmp_hash =
                            (nios2_opcode_hash *)
                            malloc(sizeof(nios2_opcode_hash));
                        nios2_ps_hash[i] = tmp_hash;
                    } else {
                        while (tmp_hash->next != NULL) {
                            tmp_hash = tmp_hash->next;
                        }
                        tmp_hash->next =
                            (nios2_opcode_hash *)
                            malloc(sizeof(nios2_opcode_hash));
                        tmp_hash = tmp_hash->next;
                    }
                    if (tmp_hash == NULL) {
                        fprintf(stderr,
                            "error allocating memory...broken disassembler\n");
                        abort();
                    }
                    tmp_hash->opcode = op;
                    tmp_hash->next = NULL;
                }
            } else if (i == ((op->match >> OP_SH_OP) & OP_MASK_OP)) {
                tmp_hash = nios2_hash[i];
                if (tmp_hash == NULL) {
                    tmp_hash =
                        (nios2_opcode_hash *)malloc(sizeof(nios2_opcode_hash));
                    nios2_hash[i] = tmp_hash;
                } else {
                    while (tmp_hash->next != NULL) {
                        tmp_hash = tmp_hash->next;
                    }
                    tmp_hash->next =
                        (nios2_opcode_hash *)malloc(sizeof(nios2_opcode_hash));
                    tmp_hash = tmp_hash->next;
                }
                if (tmp_hash == NULL) {
                    fprintf(stderr,
                            "error allocating memory...broken disassembler\n");
                    abort();
                }
                tmp_hash->opcode = op;
                tmp_hash->next = NULL;
            }
        }
    }
    nios2_hash_init = 1;
#ifdef DEBUG_HASHTABLE
    for (i = 0; i <= OP_MASK_OP; ++i) {
        printf("index: 0x%02X    ops: ", i);
        tmp_hash = nios2_hash[i];
        if (tmp_hash != NULL) {
            while (tmp_hash != NULL) {
                printf("%s ", tmp_hash->opcode->name);
                tmp_hash = tmp_hash->next;
            }
        }
        printf("\n");
    }

    for (i = 0; i <= OP_MASK_OP; ++i) {
        printf("index: 0x%02X    ops: ", i);
        tmp_hash = nios2_ps_hash[i];
        if (tmp_hash != NULL) {
            while (tmp_hash != NULL) {
                printf("%s ", tmp_hash->opcode->name);
                tmp_hash = tmp_hash->next;
            }
        }
        printf("\n");
    }
#endif
}

/* Function which returns a pointer to an nios2_opcode struct for
   a given instruction opcode, or NULL if there is an error */

const struct nios2_opcode *
nios2_find_opcode_hash(unsigned long opcode)
{
    nios2_opcode_hash *entry;

    /* Build a hash table to shorten the search time. */
    if (!nios2_hash_init) {
        nios2_init_opcode_hash();
    }

    /* first look in the pseudo-op hashtable */
    entry = nios2_ps_hash[(opcode >> OP_SH_OP) & OP_MASK_OP];

    /* look for a match and if we get one, this is the instruction we decode */
    while (entry != NULL) {
        if ((entry->opcode->match) == (opcode & entry->opcode->mask)) {
            return entry->opcode;
        } else {
            entry = entry->next;
        }
    }

    /* if we haven't yet returned, then we need to look in the main
       hashtable */
    entry = nios2_hash[(opcode >> OP_SH_OP) & OP_MASK_OP];

    if (entry == NULL) {
        return NULL;
    }

    while (entry != NULL) {
        if ((entry->opcode->match) == (opcode & entry->opcode->mask)) {
            return entry->opcode;
        } else {
            entry = entry->next;
        }
    }

    return NULL;
}

/* nios2_disassemble does all the work of disassembling a New Jersey
       instruction opcode */

int
nios2_disassemble(bfd_vma address, unsigned long opcode,
                  disassemble_info *info)
{
    const struct nios2_opcode *op;
    const char *argstr;

    info->bytes_per_line = INSNLEN;
    info->bytes_per_chunk = INSNLEN;
    info->display_endian = info->endian;
    info->insn_info_valid = 1;
    info->branch_delay_insns = 0;
    info->data_size = 0;
    info->insn_type = dis_nonbranch;
    info->target = 0;
    info->target2 = 0;

    (*info->fprintf_func) (info->stream, "%08x %08x   ",
                           (int)address, (int)opcode);

    /* Find the major opcode and use this to disassemble
       the instruction and its arguments */
    op = nios2_find_opcode_hash(opcode);

    if (op != NULL) {
        bfd_boolean is_nop = FALSE;
        if (op->pinfo == NIOS2_INSN_MACRO_MOV) {
            /* check for mov r0, r0 and if it is
               change to nop */
            int dst, src;
            dst = GET_INSN_FIELD(RRD, opcode);
            src = GET_INSN_FIELD(RRS, opcode);
            if (dst == 0 && src == 0) {
                (*info->fprintf_func) (info->stream, "nop");
                is_nop = TRUE;
            } else {
                (*info->fprintf_func) (info->stream, "%s", op->name);
            }
        } else {
          (*info->fprintf_func) (info->stream, "%s", op->name);
        }

        if (!is_nop) {
            argstr = op->args;
            if (argstr != NULL && *argstr != '\0') {
                (*info->fprintf_func) (info->stream, "\t");
                while (*argstr != '\0') {
                    nios2_print_insn_arg(argstr, opcode, address, info);
                    ++argstr;
                }
            }
        }
    } else {
        /* Handle undefined instructions. */
        info->insn_type = dis_noninsn;
        (*info->fprintf_func) (info->stream, "0x%x", (int)opcode);
    }
    /* this tells the caller how far to advance the program counter */
    return INSNLEN;
}

/* The function nios2_print_insn_arg uses the character pointed
   to by argptr to determine how it print the next token or separator
   character in the arguments to an instruction */
int
nios2_print_insn_arg(const char *argptr,
                     unsigned long opcode, bfd_vma address,
                     disassemble_info *info)
{
    unsigned long i = 0;
    unsigned long reg_base;

    assert(argptr != NULL);
    assert(info != NULL);

    switch (*argptr) {
    case ',':
    case '(':
    case ')':
        (*info->fprintf_func) (info->stream, "%c", *argptr);
        break;
    case 'd':
        i = GET_INSN_FIELD(RRD, opcode);

        if (GET_INSN_FIELD(OP, opcode) == OP_MATCH_CUSTOM
            && GET_INSN_FIELD(CUSTOM_C, opcode) == 0) {
            reg_base = COPROCREGBASE;
        } else {
            reg_base = 0;
        }

        if (i < NUMREGNAMES) {
            (*info->fprintf_func) (info->stream, "%s",
                                   nios2_regs[i + reg_base].name);
        } else {
            (*info->fprintf_func) (info->stream, "unknown");
        }
        break;
    case 's':
        i = GET_INSN_FIELD(RRS, opcode);

        if (GET_INSN_FIELD(OP, opcode) == OP_MATCH_CUSTOM
            && GET_INSN_FIELD(CUSTOM_A, opcode) == 0) {
            reg_base = COPROCREGBASE;
        } else {
            reg_base = 0;
        }

        if (i < NUMREGNAMES) {
            (*info->fprintf_func) (info->stream, "%s",
                                   nios2_regs[i + reg_base].name);
        } else {
            (*info->fprintf_func) (info->stream, "unknown");
        }
        break;
    case 't':
        i = GET_INSN_FIELD(RRT, opcode);

        if (GET_INSN_FIELD(OP, opcode) == OP_MATCH_CUSTOM
            && GET_INSN_FIELD(CUSTOM_B, opcode) == 0) {
            reg_base = COPROCREGBASE;
        } else {
            reg_base = 0;
        }

        if (i < NUMREGNAMES) {
            (*info->fprintf_func) (info->stream, "%s",
                                   nios2_regs[i + reg_base].name);
        } else {
            (*info->fprintf_func) (info->stream, "unknown");
        }
        break;
    case 'i':
        /* 16-bit signed immediate */
        i = (signed) (GET_INSN_FIELD(IMM16, opcode) << 16) >> 16;
        (*info->fprintf_func) (info->stream, "%d", (int)i);
        break;
    case 'u':
        /* 16-bit unsigned immediate */
        i = GET_INSN_FIELD(IMM16, opcode);
        (*info->fprintf_func) (info->stream, "%d", (int)i);
        break;
    case 'o':
        /* 16-bit signed immediate address offset */
        i = (signed) (GET_INSN_FIELD(IMM16, opcode) << 16) >> 16;
        address = address + 4 + i;        /* NG changed to byte offset 1/9/03 */
        (*info->print_address_func) (address, info);
        break;
    case 'p':
        /* 5-bit unsigned immediate */
        i = GET_INSN_FIELD(CACHE_OPX, opcode);
        (*info->fprintf_func) (info->stream, "%d", (int)i);
        break;
    case 'j':
        /* 5-bit unsigned immediate */
        i = GET_INSN_FIELD(IMM5, opcode);
        (*info->fprintf_func) (info->stream, "%d", (int)i);
        break;
    case 'l':
        /* 8-bit unsigned immediate */
        /* FIXME - not yet implemented */
        i = GET_INSN_FIELD(CUSTOM_N, opcode);
        (*info->fprintf_func) (info->stream, "%u", (int)i);
        break;
    case 'm':
        /* 26-bit unsigned immediate */
        i = GET_INSN_FIELD(IMM26, opcode);
        /* this translates to an address because its only used in
           call instructions */
        address = (address & 0xf0000000) | (i << 2);
        (*info->print_address_func) (address, info);
        break;
    case 'c':
        i = GET_INSN_FIELD(IMM5, opcode);        /* ctrl register index */
        (*info->fprintf_func) (info->stream, "%s",
                               nios2_regs[CTLREGBASE + i].name);
        break;
    case 'b':
        i = GET_INSN_FIELD(IMM5, opcode);
        (*info->fprintf_func) (info->stream, "%d", (int)i);
        break;
    default:
        (*info->fprintf_func) (info->stream, "unknown");
        break;
    }
    return 0;
}
