#ifndef GLOBALS_H_
#define GLOBALS_H_

#include "includes.h"

#define CPU_BITS 32
#define BITS_PER_UNIT 8
#define MAX_ADDR 0xFFFF
#define UNITS_PER 4
#define V_BITS 1

// IF components

extern BusALU addr_alu;
extern StorageObject pc;
extern StorageObject pc_mux;
extern Memory instr_mem;
extern Bus instr_abus;
extern Bus if_pc_thru;
extern Bus mux_to_pc;
extern Bus mux_to_ifid;
extern Bus jump_pc_bus;
extern BusALU ir_immed_to_ifid_signexted_imm;
extern StorageObject const_addr_inc;
extern StorageObject const_valid_on;
extern StorageObject const_valid_off;
extern StorageObject const_two;
extern StorageObject jump_reg;

// IF/ID components

typedef struct ifidreg {
  Clearable *v;
  StorageObject *ir; 
  StorageObject *pc;
  StorageObject *npc;
}IfIdReg;

extern IfIdReg ifid;


// ID components

extern StorageObject *reg_file[32];
extern BusALU sign_extend_alu;
extern BusALU branch_alu;
extern StorageObject const_sign_extend_mask;
extern StorageObject const_eight;
extern StorageObject sign_extend_imm;
extern Bus imm_bus;
extern Bus z_fill_imm_bus;
extern Bus op1_bus;
extern Bus op2_bus;
extern Bus jump_reg_thru;
extern Bus jump_reg_thru32;
extern Bus id_ir_thru;
extern Bus id_pc_thru;
extern Bus id_npc_thru;
extern Bus id_v_thru;

// ID/EX components

typedef struct idexreg {
  Clearable *v;
  StorageObject *ir;
  StorageObject *pc;
  StorageObject *npc;
  StorageObject *a;
  StorageObject *b;
  StorageObject *imm;
}IdExReg;

extern IdExReg idex;

// EX components

extern BusALU ex_alu;
extern StorageObject shift_amt;
extern StorageObject *shifty_boys[32];
extern Bus rs_lower5;
extern Bus b_thru;
extern Bus ex_ir_thru;
extern Bus ex_imm_thru;
extern Bus ex_pc_thru;
extern Bus ex_npc_thru;
extern Bus ex_v_thru;

// EX/MEM components

typedef struct exmemreg {
  Clearable *v;
  Clearable *cond;
  StorageObject *ir;
  StorageObject *pc;
  StorageObject *npc;
  StorageObject *alu_out;
  StorageObject *a;
  StorageObject *b;
  StorageObject *imm;
}ExMemReg;

extern ExMemReg exmem;

// MEM components

extern Memory data_mem;
extern Bus mem_abus;
extern Bus mem_ir_thru;
extern Bus mem_pc_thru;
extern Bus mem_npc_thru;
extern Bus mem_v_thru;
extern Bus mem_alu_out_thru;
extern Bus mem_op_b_thru;
extern Bus valid_bus;

// MEM/WB components

typedef struct memwbreg {
  Clearable *v;
  StorageObject *ir;
  StorageObject *pc;
  StorageObject *npc;
  StorageObject *alu_out;
  StorageObject *lmd;
  StorageObject *a;
  StorageObject *b;
  StorageObject *imm;
}MemWbReg;

extern MemWbReg memwb;

// WB components

extern Bus wb_bus;

extern bool done;
#endif
