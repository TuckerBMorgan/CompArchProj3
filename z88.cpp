#include <string.h>
#include <stdio.h>
#include <iostream>
#include "includes.h"
#include "globals.h"


//NOP J JAL BEQ BNE ADDI SLTI ANDI ORI XORI LUI LW SW
//HALT JR JALR BREAK ADD SUB AND OR XOR SLT SLTU SLL SRL SRA SLLV SRLV SRAV

void if_stage_first_clock() {
    instr_abus.IN().pullFrom(pc);
    instr_mem.MAR().latchFrom(instr_abus.OUT());
}

void if_stage_second_clock() {

    //all instructions have the opcode in the first 6 bits
    //we only need the top three to see what kinda of instruction it is
    long upper_opcode = (*ifid.ir)(31, 29);    //the * is required so we can deferance and get to the opcode

    long rs = (*ifid.ir)(25, 21);//IF/ID.ir(rs)
    long rt = (*ifid.ir)(20, 16);//IF/ID.ir(rt)

    bool are_equal = reg_file[rs]->value() == reg_file[rt]->value();
    long notEqual_or_equal = (*ifid.ir)(28, 26);
    
    //all branch instructions start with one of this two opcodes
    if(upper_opcode == 7 && ( (notEqual_or_equal == 4 && are_equal) || (notEqual_or_equal == 5 && !are_equal))) {//if ((IF/ID.opcode == branch)
        /*
        (( IF/ID.NPC + (sign-extend(IF/ID.IR[imm])))
        */
        addr_alu.OP1().pullFrom(*ifid.npc);
        addr_alu.OP2().pullFrom(*idex.imm);
    } 
    else {
        //else
        //(PC + 4)
        addr_alu.OP1().pullFrom(pc);
        addr_alu.OP2().pullFrom(*idex.imm);
    }

    //IF/ID.NPC and PC ←

    addr_alu.perform(BusALU::op_add);
    ifid.npc->latchFrom(addr_alu.OUT());
    pc.latchFrom(addr_alu.OUT());

    //IF/ID.IR ← Mem[PC];
    instr_mem.read();
    ifid.ir->latchFrom(instr_mem.READ());
 
}

void id_stage_first_clock() {
    //this function was intentionally left blank
	sign_extend_alu.OP1().pullFrom(*ifid.ir);
	sign_extend_alu.OP2().pullFrom(const_sign_extend_mask);
	sign_extend_alu.perform(BusALU::op_extendSign);
	idex.imm->latchFrom(sign_extend_alu.OUT());
}


void id_stage_second_clock() {

    StorageObject* a = reg_file[(*ifid.ir)(25, 21)];
    StorageObject* b = reg_file[(*ifid.ir)(20, 16)];

    op1_bus.IN().pullFrom(*a);
    op2_bus.IN().pullFrom(*b);
    id_ir_thru.IN().pullFrom(*ifid.ir);
    id_pc_thru.IN().pullFrom(*ifid.pc);
    id_npc_thru.IN().pullFrom(*ifid.npc);
    id_v_thru.IN().pullFrom(*ifid.v);

    idex.ir->latchFrom(id_ir_thru.OUT());//ID/EX.IR ↞ IF/ID.IR;
    idex.pc->latchFrom(id_pc_thru.OUT());//IDEX.PC <- IF/ID.PC;
    idex.npc->latchFrom(id_npc_thru.OUT());//IDEX.NPC <- IF/ID.NPC
    idex.v->latchFrom(id_v_thru.OUT());//IDEX.V <- IF/ID.V
    idex.a->latchFrom(op1_bus.OUT());//ID/EX.A ← reg[IF/ID.IR[rs]];
    idex.b->latchFrom(op2_bus.OUT());//ID/EX.B ← reg[IF/ID.IR[rt]];
}

void ex_stage_first_clock() {
    ex_alu.perform(BusALU::op_not);
}

void ex_stage_second_clock() {


    long ir_type = 0;
   if(ir_type == 0) {//FAKE ALU THIS NEEDS ACTUAL VALUE

     //ALU
    /*
        EX/MEM.IR ← ID/EX.IR;
        if R-R EX/MEM.ALUOutput ← ID/EX.A func ID/EX.B;
        else   EX/MEM.ALUOutput ← ID/EX.A func ID/EX.Imm;
    */
  
        ex_ir_thru.IN().pullFrom(*idex.ir);
        exmem.ir->latchFrom(ex_ir_thru.OUT());//this wil get excuted when we perform the alu op

        ex_alu.OP1().pullFrom(*idex.a);
        int is_slt_or_not = (int)(*idex.ir)(5, 3);//two two instructions slt, and sltu, have different 5-> 3 bits, so lets check for them and set the alu_op to compare
        int func = (int)(*idex.ir)(2, 0);//this is the operation we are performing with ex_alu
        if(is_slt_or_not) {
            switch (func) {
                case 0:
                case 1:
                    //ADD, and ADDU
                    ex_alu.perform(BusALU::op_add);
                break;
                case 2:
                case 3:
                    //SUB, and SUBU
                    ex_alu.perform(BusALU::op_add);
                break;
                case 4:
                    //AND
                    ex_alu.perform(BusALU::op_and);
                break;
                case 5:
                    //OR
                    ex_alu.perform(BusALU::op_or);
                break;
                case 6:
                    //XOR
                    ex_alu.perform(BusALU::op_xor);
                break;
                case 7:


                //JONATHAN!!!! a good place for you to start
                //nor isnt a op that BusALU can do, so need to find solution to this
//                    ex_alu.perform(BusALU::op_nor);
                break;
            }
        }
        else {
            
        }

        exmem.alu_out->latchFrom(ex_alu.OUT());
   }


   if(ir_type == 1) {//FAKE FAKE FAKE FAKE LOAD STORE FLAG VALUE

        //EX/MEM.IR ← ID/EX.IR;
        ex_ir_thru.IN().pullFrom(*idex.ir);
        exmem.ir->latchFrom(ex_ir_thru.OUT());
        
        //EX/MEM.B ← ID/EX.B;
        b_thru.IN().pullFrom(*idex.b);
        exmem.b->latchFrom(b_thru.OUT());

        //EX/MEM.ALUOutput ← ID/EX.A + ID/EX.Imm;
        ex_alu.OP1().pullFrom(*idex.a);
        ex_alu.OP2().pullFrom(*idex.imm);
        ex_alu.perform(BusALU::op_add);
        exmem.alu_out->latchFrom(ex_alu.OUT());
   }



   if(ir_type == 2) {//THIS IS FAKE SO FAKE THE FAKEST
        //Branch
        // EXS/MEM.ALUOutput ← ID/EX.NPC + ID/EX.Imm;
        ex_alu.OP1().pullFrom(*idex.npc);
        ex_alu.OP2().pullFrom(*idex.imm);//THIS MIGHT BE VERY WRONG
        ex_alu.perform(BusALU::op_add);
        exmem.alu_out->latchFrom(ex_alu.OUT());
        //    EX/MEM.cond ← (ID/EX.A == 0);
        if(idex.a->value() == 0) {
            exmem.cond->set();
        }
        else {
            exmem.cond->clear();
        }
   }
}

void mem_stage_first_clock() {
    int fake_op_dis = 0;
    if(fake_op_dis == 0) {//ALU OPCODE

        /*
        MEM/WB.IR ← EX/MEM.IR;
        MEM/WB.ALUOutput ← EX/MEM.ALUOutput;
        */

        mem_ir_thru.IN().pullFrom(*exmem.ir);
        mem_alu_out_thru.IN().pullFrom(*exmem.ir);
        memwb.ir->latchFrom(mem_ir_thru.OUT());
        memwb.alu_out->latchFrom(mem_alu_out_thru.OUT());
   //     Clock::tick();
    }
    else if (fake_op_dis == 1) {
        /*
        Load/store instructions:
        MEM/WB.IR ← EX/MEM.IR;
        */
        mem_ir_thru.IN().pullFrom(*exmem.ir);
        memwb.ir->latchFrom(mem_ir_thru.OUT());
        mem_abus.IN().pullFrom(*exmem.alu_out);
        data_mem.MAR().latchFrom(mem_abus.OUT());

        if(1) {//if Load
            //MEM/WB.LMD ← Mem[EX/MEM.ALUOutput]
            data_mem.read();
            memwb.lmd->latchFrom(data_mem.READ());
            Clock::tick();
        }
        else if(2) {

        }


    }
    else if (fake_op_dis == 3) {
        /*
        Branch instructions:
            Idle 
        //this page left intentionally left blank
        */
    }
}

void mem_stage_second_clock() {
    int fake_op_dis = 0;


    if(fake_op_dis == 0) {

    }
    else if(fake_op_dis == 1) {

        if(1) {//load 

        }
        else {//store

        }

    }
    else if(fake_op_dis == 2) {

    }
}


void wb() {
    /*
ALU instructions:

    reg[MEM/WB.IR[rd]] ← MEM/WB.ALUOutput;
      or
    reg[MEM/WB.IR[rt]] ← MEM/WB.ALUOutput;

Load/store instructions:

    if Load reg[MEM/WB.IR[rt]] ← MEM/WB.LMD;

Branch instructions:
    Idle 
    */

   int fake_op_dis = 0;

   if (fake_op_dis == 0) {
        /*
         reg[MEM/WB.IR[rd]] ← MEM/WB.ALUOutput;
          or
         reg[MEM/WB.IR[rt]] ← MEM/WB.ALUOutput;
        */
//       wb_bus.pullFrom(reg_file[(*memwb.ir)()]);

   }
   else if (fake_op_dis == 1) {

   }
}

void connect() {
    pc.connectsTo(addr_alu.OP2());
    pc.connectsTo(instr_abus.IN());
    pc.connectsTo(addr_alu.OUT());
    pc.connectsTo(branch_alu.OUT());
    instr_mem.MAR().connectsTo(instr_abus.IN());
    const_addr_inc.connectsTo(addr_alu.OP1());
    const_valid_on.connectsTo(valid_bus.IN());
    const_valid_off.connectsTo(valid_bus.IN());

    ifid.v->connectsTo(valid_bus.OUT());
    ifid.v->connectsTo(id_v_thru.IN());
    ifid.ir->connectsTo(instr_mem.READ());
    ifid.ir->connectsTo(id_ir_thru.IN());
    ifid.ir->connectsTo(sign_extend_alu.OP1());
    sign_extend_imm.connectsTo(sign_extend_alu.OUT());
    sign_extend_imm.connectsTo(branch_alu.OP1());
    sign_extend_imm.connectsTo(imm_bus.IN());
    ifid.pc->connectsTo(addr_alu.OUT());
    //  ifid.pc->connectsTo(branch_alu.OUT());
    ifid.pc->connectsTo(branch_alu.OP2());
    ifid.pc->connectsTo(id_pc_thru.IN());
    ifid.npc->connectsTo(addr_alu.OUT());
    ifid.npc->connectsTo(branch_alu.OUT());
    ifid.npc->connectsTo(branch_alu.OP2());
    ifid.npc->connectsTo(id_npc_thru.IN());
    const_sign_extend_mask.connectsTo(sign_extend_alu.OP2());
    for(int i = 0; i < 32; ++i) {
    reg_file[i]->connectsTo(op1_bus.IN());
    reg_file[i]->connectsTo(op2_bus.IN());
    reg_file[i]->connectsTo(wb_bus.OUT());
    }

    idex.v->connectsTo(id_v_thru.OUT());
    idex.v->connectsTo(ex_v_thru.IN());
    idex.ir->connectsTo(id_ir_thru.OUT());
    idex.ir->connectsTo(ex_ir_thru.IN());
    idex.pc->connectsTo(id_pc_thru.OUT());
    idex.pc->connectsTo(ex_pc_thru.IN());
    idex.npc->connectsTo(id_npc_thru.OUT());
    idex.npc->connectsTo(ex_npc_thru.IN());
    idex.a->connectsTo(op1_bus.OUT());
    idex.a->connectsTo(ex_alu.OP1());
    idex.b->connectsTo(op2_bus.OUT());
    idex.b->connectsTo(b_thru.IN());
    idex.b->connectsTo(ex_alu.OP2());
    idex.imm->connectsTo(imm_bus.OUT());
    idex.imm->connectsTo(ex_alu.OP2());

    //TODO COND?????
    exmem.v->connectsTo(ex_v_thru.OUT());
    exmem.v->connectsTo(mem_v_thru.IN());
    exmem.ir->connectsTo(ex_ir_thru.OUT());
    exmem.ir->connectsTo(mem_ir_thru.IN());
    exmem.pc->connectsTo(ex_pc_thru.OUT());
    exmem.pc->connectsTo(mem_pc_thru.IN());
    exmem.npc->connectsTo(ex_npc_thru.OUT());
    exmem.npc->connectsTo(mem_npc_thru.IN());
    exmem.b->connectsTo(b_thru.OUT());
    exmem.b->connectsTo(mem_abus.IN());
    exmem.alu_out->connectsTo(ex_alu.OUT());
    exmem.alu_out->connectsTo(mem_alu_out_thru.IN());
    exmem.alu_out->connectsTo(mem_abus.IN());
    exmem.alu_out->connectsTo(data_mem.WRITE());
    data_mem.MAR().connectsTo(mem_abus.OUT());

    memwb.v->connectsTo(mem_v_thru.OUT());
    memwb.ir->connectsTo(mem_ir_thru.OUT());
    memwb.pc->connectsTo(mem_pc_thru.OUT());
    memwb.npc->connectsTo(mem_npc_thru.OUT());
    memwb.alu_out->connectsTo(mem_alu_out_thru.OUT());
    memwb.alu_out->connectsTo(wb_bus.IN());
    memwb.lmd->connectsTo(data_mem.READ());
    memwb.lmd->connectsTo(wb_bus.IN());
}

int main() {
    connect();

    return 0;
};
