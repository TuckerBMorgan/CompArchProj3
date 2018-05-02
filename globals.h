///
/// globals.cpp
///
/// T. Morgan, J. Schenk
///
/// Defines hardware global vars.
///
///////////////////////////////////////////////////////////////////////////////

#ifndef GLOBALS_H_
#define GLOBALS_H_

#include "includes.h"

#define CPU_BITS 32      // Size of most things
#define BITS_PER_UNIT 8  // For instantiating memory objects
#define MAX_ADDR 0xFFFF  // Size of memory
#define UNITS_PER 4      // Units per read/write of mem
#define V_BITS 1         // Size of v registers

// IF components

extern BusALU addr_alu;      // ALU for incrementing PC
extern StorageObject pc;     // Program counter
extern Memory instr_mem;     // Instruction memory
extern Bus instr_abus;       // Loads MAR
extern Bus if_pc_thru;       // Loads PC into ifid pc
extern Bus injection_bus;    // Moves NOPs around
extern Bus jump_pc_bus;      // Feeds branch addr into PC
extern StorageObject const_addr_inc; // Holds a 4 to increment PC
extern StorageObject const_two;
extern StorageObject jump_reg; // Holds addr we are jumping to
extern StorageObject const_nop_value;//used for injecting nops into pipelines

// IF/ID components

typedef struct ifidreg {
  Clearable *v;       // valid
  StorageObject *ir;  // ir
  StorageObject *pc;  // program counter
  StorageObject *npc; // new program counter
}IfIdReg;

extern IfIdReg ifid; //IF/ID pipeline reg


// ID components

extern StorageObject *reg_file[32]; // GPR file
extern BusALU sign_extend_alu;      // ALU for sign extension
extern BusALU branch_alu;           // ALU for calculating branch addrs
extern StorageObject const_sign_extend_mask; // Constant reg for sign extension
extern StorageObject const_eight;       // Used to increment NPC for jump & links
extern StorageObject sign_extend_imm;   // Holds the sign extended immediate vals
extern Bus imm_bus;                     // Sends imm to ifid imm
extern Bus z_fill_imm_bus;              // zero fill immediate bus
extern Bus op1_bus;                 // operand 1
extern Bus op2_bus;                 // operand 2
extern Bus jump_reg_thru;           // carries 16 bit imm to jump reg
extern Bus jump_reg_thru32;         // carries 32 bit imm to jump reg
// These carry data to the next pipeline register
extern Bus id_ir_thru;
extern Bus id_pc_thru;
extern Bus id_npc_thru;
extern Bus id_v_thru;

// ID/EX components

typedef struct idexreg {
  Clearable *v;       // valid
  StorageObject *ir;  // ir
  StorageObject *pc;  // pc
  StorageObject *npc; // npc
  StorageObject *a;   // op a
  StorageObject *b;   // op b
  StorageObject *imm; // immediate
}IdExReg;

extern IdExReg idex; //ID/EX pipeline reg

// EX components

extern BusALU ex_alu;             // Execution ALU
extern StorageObject shift_amt;   // Used for shifts
extern StorageObject *shifty_boys[32]; // Used for shifts
extern Bus rs_lower5;     // thing being shifted
// Move things from EX to MEM
extern Bus b_thru;
extern Bus ex_ir_thru;
extern Bus ex_imm_thru;
extern Bus ex_pc_thru;
extern Bus ex_npc_thru;
extern Bus ex_v_thru;

// EX/MEM components

typedef struct exmemreg {
  Clearable *v;           // valid
  Clearable *cond;        // not used
  StorageObject *ir;      // ir
  StorageObject *pc;      // pc
  StorageObject *npc;     // npc
  StorageObject *alu_out; // alu output
  StorageObject *a;       // op a
  StorageObject *b;       // op b
  StorageObject *imm;     // immediate
}ExMemReg;

extern ExMemReg exmem;    // EX/MEM pipeline register

// MEM components

extern Memory data_mem; // Data memory
extern Bus mem_abus;    // data memory a bus
// Carry things to WB pipeline reg
extern Bus mem_ir_thru;
extern Bus mem_pc_thru;
extern Bus mem_npc_thru;
extern Bus mem_v_thru;
extern Bus mem_alu_out_thru;
extern Bus mem_op_b_thru;
extern Bus valid_bus;

// MEM/WB components

typedef struct memwbreg {
  Clearable *v;           // valid
  StorageObject *ir;      // ir
  StorageObject *pc;      // pc
  StorageObject *npc;     // npc
  StorageObject *alu_out; // alu output
  StorageObject *lmd;     // memory output
  StorageObject *a;       // op a
  StorageObject *b;       // op b
  StorageObject *imm;     // immediate
}MemWbReg;

extern MemWbReg memwb;   // MEM/WB pipeline reg

// WB components

extern Bus wb_bus;      // writeback bus

extern bool done;       // simulation done?

// Halt strings
extern const char *halt_instr;
extern const char *unimp_instr;
extern const char *undef_instr;

#endif
