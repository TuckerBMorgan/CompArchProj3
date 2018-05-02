///
/// globals.cpp
///
/// T. Morgan, J. Schenk
///
/// Sets up hardware global vars.
///
///////////////////////////////////////////////////////////////////////////////
#include "includes.h"
// IF components

BusALU addr_alu("ADDR_ALU", CPU_BITS);
StorageObject pc("PC", CPU_BITS);
Memory instr_mem("IMemory", CPU_BITS, BITS_PER_UNIT, MAX_ADDR, UNITS_PER);
Bus instr_abus("INSTR_ABUS", CPU_BITS);
StorageObject const_addr_inc("CONST_ADDR_INCR", CPU_BITS, 0x04);
StorageObject const_valid_on("CONST_VALID_ON", CPU_BITS, 0x01);
StorageObject const_valid_off("CONST_VALID_OFF", CPU_BITS, 0x00);
StorageObject mdr("mdr", CPU_BITS);
StorageObject jump_reg("JUMP_REG", CPU_BITS);
StorageObject const_nop_value("NOP_REG", CPU_BITS, 0x04000000);
Bus valid_bus("VALID_BUS", V_BITS);
Bus if_pc_thru("IF_PC_THRU", CPU_BITS);
Bus jump_pc_bus("JUMP_PC_BUS", CPU_BITS);
BusALU ir_immed_to_ifid_signexted_imm("IR_IMMED_TO_IFID_SIGNEXNTED_IMM", CPU_BITS / 2);
// IF/ID components 
Bus injection_bus("INJECT_BUS", CPU_BITS);
Clearable ifid_v("IFID_V", V_BITS);
StorageObject ifid_ir("IFID_IR", CPU_BITS);
StorageObject ifid_pc("IFID_PC", CPU_BITS);
StorageObject ifid_npc("IFID_NPC", CPU_BITS);
IfIdReg ifid = {
  &ifid_v, &ifid_ir, &ifid_pc, &ifid_npc
}; 

// ID components
StorageObject r0("R0", CPU_BITS, 0x00);
StorageObject r1("R1", CPU_BITS);
StorageObject r2("R2", CPU_BITS);
StorageObject r3("R3", CPU_BITS);
StorageObject r4("R4", CPU_BITS);
StorageObject r5("R5", CPU_BITS);
StorageObject r6("R6", CPU_BITS);
StorageObject r7("R7", CPU_BITS);
StorageObject r8("R8", CPU_BITS);
StorageObject r9("R9", CPU_BITS);

StorageObject r10("R10", CPU_BITS);
StorageObject r11("R11", CPU_BITS);
StorageObject r12("R12", CPU_BITS);
StorageObject r13("R13", CPU_BITS);
StorageObject r14("R14", CPU_BITS);
StorageObject r15("R15", CPU_BITS);
StorageObject r16("R16", CPU_BITS);
StorageObject r17("R17", CPU_BITS);
StorageObject r18("R18", CPU_BITS);
StorageObject r19("R19", CPU_BITS);

StorageObject r20("R20", CPU_BITS);
StorageObject r21("R21", CPU_BITS);
StorageObject r22("R22", CPU_BITS);
StorageObject r23("R23", CPU_BITS);
StorageObject r24("R24", CPU_BITS);
StorageObject r25("R25", CPU_BITS);
StorageObject r26("R26", CPU_BITS);
StorageObject r27("R27", CPU_BITS);
StorageObject r28("R28", CPU_BITS);
StorageObject r29("R29", CPU_BITS);

StorageObject r30("R30", CPU_BITS);
StorageObject r31("R31", CPU_BITS);

StorageObject *reg_file[32] = {
    &r0,
    &r1,
    &r2,
    &r3,
    &r4,
    &r5,
    &r6,
    &r7,
    &r8,
    &r9,
    &r10,
    &r11,
    &r12,
    &r13,
    &r14,
    &r15,
    &r16,
    &r17,
    &r18,
    &r19,
    &r20,
    &r21,
    &r22,
    &r23,
    &r24,
    &r25,
    &r26,
    &r27,
    &r28,
    &r29,
    &r30,
    &r31,
};

/*

*/

BusALU branch_alu("BRANCH_ALU", CPU_BITS);
BusALU sign_extend_alu("SE_ALU", CPU_BITS);
StorageObject const_sign_extend_mask("SIGN_EXTEND", CPU_BITS, 0x8000);
StorageObject const_eight("CONST_EIGHT", CPU_BITS, 8);
StorageObject sign_extend_imm("SIGN_EXTEND_IMM", CPU_BITS);
Bus imm_bus("IMM_BUS", CPU_BITS);
Bus z_fill_imm_bus("Z_FILL_IMM_BUS", CPU_BITS/2);
Bus op1_bus("OP_BUS1", CPU_BITS);
Bus op2_bus("OP_BUS2", CPU_BITS);
Bus jump_reg_thru("JUMP_REG_THRU", 26);
Bus jump_reg_thru32("JUMP_REG_THRU32", CPU_BITS);
Bus id_ir_thru("ID_IR_THRU", CPU_BITS);
Bus id_pc_thru("ID_PC_THRU", CPU_BITS);
Bus id_npc_thru("ID_NPC_THRU", CPU_BITS);
Bus id_v_thru("ID_V_THRU", CPU_BITS);

// ID/EX components

Clearable idex_v("IDEX_V", V_BITS);
StorageObject idex_ir("IDEX_IR", CPU_BITS);
StorageObject idex_pc("IDEX_PC", CPU_BITS);
StorageObject idex_npc("IDEX_NPC", CPU_BITS);
StorageObject idex_a("IDEX_A", CPU_BITS);
StorageObject idex_b("IDEX_B", CPU_BITS);
StorageObject idex_imm("IDEX_IMM", CPU_BITS);
IdExReg idex = {
  &idex_v, &idex_ir, &idex_pc, &idex_npc, &idex_a, &idex_b, &idex_imm
};

// EX components

BusALU ex_alu("EX_ALU", CPU_BITS);
StorageObject shift_amt("SHIFT_AMT", CPU_BITS);
Bus rs_lower5("RS_LOWER5", 5);
Bus b_thru("B_THRU", CPU_BITS);
Bus ex_ir_thru("EX_IR_THRU", CPU_BITS);
Bus ex_imm_thru("EX_IMM_THRU", CPU_BITS);
Bus ex_pc_thru("EX_PC_THRU", CPU_BITS);
Bus ex_npc_thru("EX_NPC_THRU", CPU_BITS);
Bus ex_v_thru("EX_V_THRU", CPU_BITS);

StorageObject s0("s0", CPU_BITS, 0);
StorageObject s1("s1", CPU_BITS, 1);
StorageObject s2("s2", CPU_BITS, 2);
StorageObject s3("s3", CPU_BITS, 3);
StorageObject s4("s4", CPU_BITS, 4);
StorageObject s5("s5", CPU_BITS, 5);
StorageObject s6("s6", CPU_BITS, 6);
StorageObject s7("s7", CPU_BITS, 7);
StorageObject s8("s8", CPU_BITS, 8);
StorageObject s9("s9", CPU_BITS, 9);

StorageObject s10("r10", CPU_BITS, 10);
StorageObject s11("s11", CPU_BITS, 11);
StorageObject s12("s12", CPU_BITS, 12);
StorageObject s13("s13", CPU_BITS, 13);
StorageObject s14("s14", CPU_BITS, 14);
StorageObject s15("s15", CPU_BITS, 15);
StorageObject s16("s16", CPU_BITS, 16);
StorageObject s17("s17", CPU_BITS, 17);
StorageObject s18("s18", CPU_BITS, 18);
StorageObject s19("s19", CPU_BITS, 19);

StorageObject s20("s20", CPU_BITS, 20);
StorageObject s21("s21", CPU_BITS, 21);
StorageObject s22("s22", CPU_BITS, 22);
StorageObject s23("s23", CPU_BITS, 23);
StorageObject s24("s24", CPU_BITS, 24);
StorageObject s25("s25", CPU_BITS, 25);
StorageObject s26("s26", CPU_BITS, 26);
StorageObject s27("s27", CPU_BITS, 27);
StorageObject s28("s28", CPU_BITS, 28);
StorageObject s29("s29", CPU_BITS, 29);

StorageObject s30("s30", CPU_BITS, 30);
StorageObject s31("s31", CPU_BITS, 31);

StorageObject *shifty_boys[32] = {
    &s0,
    &s1,
    &s2,
    &s3,
    &s4,
    &s5,
    &s6,
    &s7,
    &s8,
    &s9,
    &s10,
    &s11,
    &s12,
    &s13,
    &s14,
    &s15,
    &s16,
    &s17,
    &s18,
    &s19,
    &s20,
    &s21,
    &s22,
    &s23,
    &s24,
    &s25,
    &s26,
    &s27,
    &s28,
    &s29,
    &s30,
    &s31,
};

// EX/MEM components

Clearable exmem_v("EXMEM_V", V_BITS);
Clearable exmem_cond("EXMEM_COND", 1);
StorageObject exmem_ir("EXMEM_IR", CPU_BITS);
StorageObject exmem_pc("EXMEM_PC", CPU_BITS);
StorageObject exmem_npc("EXMEM_NPC", CPU_BITS);
StorageObject exmem_alu_out("EXMEM_ALU_OUT", CPU_BITS);
StorageObject exmem_a("EXMEM_A", CPU_BITS);
StorageObject exmem_b("EXMEM_B", CPU_BITS);
StorageObject exmem_imm("EXMEM_IMM", CPU_BITS);
ExMemReg exmem = {
  &exmem_v, &exmem_cond, &exmem_ir, &exmem_pc,
  &exmem_npc, &exmem_alu_out, &exmem_a, &exmem_b, &exmem_imm
};

// MEM components

Memory data_mem("DMemory", CPU_BITS, BITS_PER_UNIT, MAX_ADDR, UNITS_PER);
Bus mem_abus("MEM_ABUS", CPU_BITS);
Bus mem_ir_thru("MEM_IR_THRU", CPU_BITS);
Bus mem_pc_thru("MEM_PC_THRU", CPU_BITS);
Bus mem_npc_thru("MEM_NPC_THRU", CPU_BITS);
Bus mem_v_thru("MEM_V_THRU", CPU_BITS);
Bus mem_alu_out_thru("MEM_ALU_OUT_THRU", CPU_BITS);
Bus mem_op_b_thru("MEM_OP_B_THRU", CPU_BITS);

// MEM/WB components

Clearable memwb_v("MEMWB_V", V_BITS);
StorageObject memwb_ir("MEMWB_IR", CPU_BITS);
StorageObject memwb_pc("MEMWB_PC", CPU_BITS);
StorageObject memwb_npc("MEMWB_NPC", CPU_BITS);
StorageObject memwb_alu_out("MEMWB_ALU_OUT", CPU_BITS);
StorageObject memwb_lmd("MEMWB_LMD", CPU_BITS);
StorageObject memwb_a("MEMWB_A", CPU_BITS);
StorageObject memwb_b("MEMWB_B", CPU_BITS);
StorageObject memwb_imm("MEMWB_IMM", CPU_BITS);
MemWbReg memwb = {
  &memwb_v, &memwb_ir, &memwb_pc, &memwb_npc, &memwb_alu_out,
  &memwb_lmd, &memwb_a, &memwb_b, &memwb_imm
};

// WB components

Bus wb_bus("WB_BUS", CPU_BITS);

bool done(false);

const char *halt_instr = "HALT instruction executed\n";
const char *unimp_instr = "unimplemented instruction\n";
const char *undef_instr = "undefined instruction\n";
