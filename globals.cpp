#include "includes.h"
// IF components

BusALU addr_alu("ADDR_ALU", CPU_BITS);
StorageObject pc("PC", CPU_BITS);
Memory instr_mem("INSTR_MEM", CPU_BITS, BITS_PER_UNIT, MAX_ADDR, UNITS_PER);
Bus instr_abus("INSTR_ABUS", CPU_BITS);
StorageObject const_addr_inc("CONST_ADDR_INCR", CPU_BITS, 0x04);
StorageObject const_valid_on("CONST_VALID_ON", CPU_BITS, 0x01);
StorageObject const_valid_off("CONST_VALID_OFF", CPU_BITS, 0x00);
StorageObject const_two("CONST_TWO", CPU_BITS, 0x02);
StorageObject mdr("mdr", CPU_BITS);
Bus valid_bus("VALID_BUS", V_BITS);
BusALU ir_immed_to_ifid_signexted_imm("IR_IMMED_TO_IFID_SIGNEXNTED_IMM", CPU_BITS / 2);
StorageObject sixteen_bit_mask("SIXTEEN_BIT_MASK", 0x00008000);
// IF/ID components 

Clearable ifid_v("IFID_V", V_BITS);
StorageObject ifid_ir("IFID_IR", CPU_BITS);
StorageObject ifid_pc("IFID_PC", CPU_BITS);
StorageObject ifid_npc("IFID_NPC", CPU_BITS);
IfIdReg ifid = {
  &ifid_v, &ifid_ir, &ifid_pc, &ifid_npc
}; 

// ID components
StorageObject r0("r0", CPU_BITS);
StorageObject r1("r1", CPU_BITS);
StorageObject r2("r2", CPU_BITS);
StorageObject r3("r3", CPU_BITS);
StorageObject r4("r4", CPU_BITS);
StorageObject r5("r5", CPU_BITS);
StorageObject r6("r6", CPU_BITS);
StorageObject r7("r7", CPU_BITS);
StorageObject r8("r8", CPU_BITS);
StorageObject r9("r9", CPU_BITS);

StorageObject r10("r10", CPU_BITS);
StorageObject r11("r11", CPU_BITS);
StorageObject r12("r12", CPU_BITS);
StorageObject r13("r13", CPU_BITS);
StorageObject r14("r14", CPU_BITS);
StorageObject r15("r15", CPU_BITS);
StorageObject r16("r16", CPU_BITS);
StorageObject r17("r17", CPU_BITS);
StorageObject r18("r18", CPU_BITS);
StorageObject r19("r19", CPU_BITS);

StorageObject r20("r20", CPU_BITS);
StorageObject r21("r21", CPU_BITS);
StorageObject r22("r22", CPU_BITS);
StorageObject r23("r23", CPU_BITS);
StorageObject r24("r24", CPU_BITS);
StorageObject r25("r25", CPU_BITS);
StorageObject r26("r26", CPU_BITS);
StorageObject r27("r27", CPU_BITS);
StorageObject r28("r28", CPU_BITS);
StorageObject r29("r29", CPU_BITS);

StorageObject r30("r30", CPU_BITS);
StorageObject r31("r31", CPU_BITS);

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
StorageObject const_sign_extend_mask("SIGN_EXTEND", CPU_BITS, 0x10000);
StorageObject sign_extend_imm("SIGN_EXTEND_IMM", CPU_BITS);
Bus imm_bus("IMM_BUS1", CPU_BITS);
Bus op1_bus("OP_BUS1", CPU_BITS);
Bus op2_bus("OP_BUS2", CPU_BITS);
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
Bus b_thru("B_THRU", CPU_BITS);
Bus ex_ir_thru("EX_IR_THRU", CPU_BITS);
Bus ex_pc_thru("EX_PC_THRU", CPU_BITS);
Bus ex_npc_thru("EX_NPC_THRU", CPU_BITS);
Bus ex_v_thru("EX_V_THRU", CPU_BITS);

// EX/MEM components

Clearable exmem_v("EXMEM_V", V_BITS);
StorageObject exmem_cond("EXMEM_COND", CPU_BITS);
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

Memory data_mem("DATA_MEM", CPU_BITS, BITS_PER_UNIT, MAX_ADDR, UNITS_PER);
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
