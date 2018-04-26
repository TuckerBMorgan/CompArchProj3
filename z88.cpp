#include <string.h>
#include <stdio.h>
#include <iostream>
#include "includes.h"
#include "globals.h"


//NOP J JAL LUI LW SW
//HALT JR JALR BREAK SLT SLTU


//done is ex stage ADDI, ANDI, ORI, XORI, ADD, SUB, AND, OR, XOR, SLL, SRL, SRA, SLLV, SRLV, SRAV
//SLTI, BEQ, BNE

enum OPCodeClass {
    ALU,
    BRANCH,
    LOAD,
    STORE,
    NOP,
    JUMP,
    HALT,
    BREAK
};


OPCodeClass from_full_instruction_return_opcode_class(StorageObject* ir) {
    long higher_op = (*ir)(31, 26);

    long higher_higher_op = (*ir)(31, 29);
    long lower_higher_op = (*ir)(28, 26);

    long func_higher_op = (*ir)(5, 3);
    long func_lower_op = (*ir)(2, 0);

    if(higher_op == 39) return ALU;

    if(higher_op == 0) {//all special instrctions
        if(func_higher_op == 0){
            if(func_lower_op == 0) {
                return HALT;
            }
            else if(func_lower_op == 2 || func_lower_op == 3) {
                return JUMP;
            }
            else if(func_lower_op == 7) {
                return BREAK;
            }
        }

        //all other special instructions are ALU
        return ALU;
    }
    else {

        if(higher_higher_op == 0) {
            if(lower_higher_op == 1) {
                return NOP;
            }
            else if(lower_higher_op == 2 || lower_higher_op == 3) {
                return JUMP;
            }
        }

        if(higher_higher_op >= 2 && higher_higher_op < 4) {
            return ALU;
        }

        if(higher_higher_op == 4) {
            return LOAD;
        }
        if (higher_higher_op == 5) {
            return STORE;
        }
        else {
            return BRANCH;
        }
    }
};


const char* opcode_list_1[][8] = {
    {"-", "NOP    ", "J      ", "JAL    ", "-", "-", "-","-"},
    {"-", "-", "-", "-", "-", "-", "-","-"},
    {"ADDI   ", "ADDIU  ", "-", "-", "ANDI   ", "ORI    ", "XORI   ","-"},
    {"SLTI   ", "SLTIU  ", "-", "-", "-", "-", "-","-"},
    {"LB     ", "LH     ", "-", "LW     ", "LBU    ", "LHU    ", "-","LUI    "},
    {"SB     ", "SH     ", "-", "SW     ", "-", "-", "-","-"},
    {"-", "-", "BLTZ   ", "BLTZAL ", "-", "-", "-","-"},
    {"-", "-", "BGEZ   ", "BGEZAL ", "BEQ    ", "BNE    ","BLEZ   ", "BGTZ   "},
};

const char* opcode_list_2[][8] = {
    {"HALT   ", "-", "JR     ", "JALR   ", "-", "-", "SYSCALL","BREAK  "},
    {"-", "-", "-", "-", "-", "-", "-","-"},
    {"ADD    ", "ADDU   ", "SUB    ", "SUBU   ", "AND    ", "OR     ", "XOR    ","NOR    "},
    {"SLT    ", "SLTU   ", "-", "-", "-", "-", "-","-"},
    {"-", "-", "-", "-", "-", "SLL    ", "SRL    ","SRA    "},
    {"-", "-", "-", "-", "-", "SLLV   ", "SRLV   ","SRAV   "},
    {"-", "-", "-", "-", "-", "-","-", "-"},
    {"-", "-", "-", "-", "-", "-","-", "-"}
};

const char* rnames[32] = {
    "R0", "R1", "R2", "R3", "R4", "R5", "R6", "R7", "R8",
    "R9", "R10", "R11", "R12", "R13", "R14", "R15", "R16",
    "R17", "R18", "R19", "R20", "R21", "R22", "R23", "R24",
    "R25", "R26", "R27", "R28", "R29", "R30", "R31"
};

const char* get_opcode_string_from_ir(StorageObject* ir) {
    long opcode = (*ir)(31, 26);
    if(opcode == 0) {
        long upper = (*ir)(5, 3);
        long down = (*ir)(2, 0);
        return opcode_list_2[upper][down];
    }
    else {
        long upper = (*ir)(31, 29);
        long down = (*ir)(28, 26);

        return opcode_list_1[upper][down];
    }

}

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
        branch_alu.OP2().pullFrom(*ifid.npc);
        branch_alu.OP1().pullFrom(sign_extend_imm);

        //IF/ID.NPC and PC ←
        branch_alu.perform(BusALU::op_add);
        ifid.npc->latchFrom(branch_alu.OUT());
        pc.latchFrom(branch_alu.OUT());
    }
    else {
        //else
        //(PC + 4)
        addr_alu.OP2().pullFrom(pc);
        addr_alu.OP1().pullFrom(const_addr_inc);

        //IF/ID.NPC and PC ←
        addr_alu.perform(BusALU::op_add);
        ifid.npc->latchFrom(addr_alu.OUT());
        pc.latchFrom(addr_alu.OUT());
    }

    if_pc_thru.IN().pullFrom(pc);
    ifid.pc->latchFrom(if_pc_thru.OUT());

    //IF/ID.IR ← Mem[PC];
    instr_mem.read();
    ifid.ir->latchFrom(instr_mem.READ());
    ifid.v->set();
}

void id_stage_first_clock() {
  //this function was intentionally left blank
    if(ifid.v->value() == 0) return;
	  sign_extend_alu.OP1().pullFrom(*ifid.ir);
	  sign_extend_alu.OP2().pullFrom(const_sign_extend_mask);
	  sign_extend_alu.perform(BusALU::op_extendSign);
    sign_extend_imm.latchFrom(sign_extend_alu.OUT());
	  //idex.imm->latchFrom(sign_extend_alu.OUT());
}

void id_stage_second_clock() {
    if(ifid.v->value() == 0) return;

    StorageObject* a = reg_file[(*ifid.ir)(25, 21)];//Rs
    StorageObject* b = reg_file[(*ifid.ir)(20, 16)];//Rt

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
    long op = (*ifid.ir)(31, 26);
    if( op >= 20 && op <= 22 ) {
        z_fill_imm_bus.IN().pullFrom(*ifid.ir);
	      idex.imm->latchFrom(z_fill_imm_bus.OUT());
    }
    else {
        imm_bus.IN().pullFrom(sign_extend_imm);
	      idex.imm->latchFrom(imm_bus.OUT());
    }
}

void ex_stage_first_clock() {
    if(idex.v->value() == 0) return;
    long is_special = (*idex.ir)(31, 26);
    long is_nor = (*idex.ir)(5, 0);

    if(is_special == 0 && is_nor == 23) {//if this is a nor operation, lets not it and then drop it back into idex.a, 23 is the func code for nor
        ex_alu.OP1().pullFrom(*idex.a);
        ex_alu.perform(BusALU::op_not);
        idex.a->latchFrom(ex_alu.OUT());
    }
    rs_lower5.IN().pullFrom(*(idex.a));
    shift_amt.latchFrom(rs_lower5.OUT());
}

void ex_stage_second_clock() {
    if(idex.v->value() == 0) return;
    //only ALU and Load/Store opreations require we do this, but at the same time there is nore doing this for the branch insturctions as it doe not use the IR register
    ex_ir_thru.IN().pullFrom(*idex.ir);
    exmem.ir->latchFrom(ex_ir_thru.OUT());//this wil get excuted when we perform the alu op
    ex_v_thru.IN().pullFrom(*idex.v);
    exmem.v->latchFrom(ex_v_thru.OUT());
    ex_pc_thru.IN().pullFrom(*idex.pc);
    exmem.pc->latchFrom(ex_pc_thru.OUT());
    ex_npc_thru.IN().pullFrom(*idex.npc);
    exmem.npc->latchFrom(ex_npc_thru.OUT());
    ex_imm_thru.IN().pullFrom(*idex.imm);
    exmem.imm->latchFrom(ex_imm_thru.OUT());


    long ir_type = (*idex.ir)(31, 26);
    long func_value = (*idex.ir)(5, 0);

    //FIRST WE CHECK FOR THE SPECIAL ALU OPS
    if( (ir_type  == 0 && func_value >= 16) || (ir_type >= 16 && ir_type < 32) || ir_type == 39) {//FAKE ALU THIS NEEDS ACTUAL VALUE
        long is_shifts = (*idex.ir)(5, 0);
        long two_o = (*idex.ir)(2, 0);
        long five_three = (*idex.ir)(5, 3);

        if(ir_type == 0 && is_shifts >= 37){//ALLL SHIFT OPERATIONS

            ex_alu.OP1().pullFrom(*idex.b);

            if(five_three == 4) {
                long shft = (*idex.ir)(10, 6);
                ex_alu.OP2().pullFrom(*(shifty_boys[shft]));
            }
            else if(five_three == 5) {
                ex_alu.OP2().pullFrom(shift_amt);
            }

            if(two_o == 5) {
                ex_alu.perform(BusALU::op_lshift);
            }
            else if(two_o == 6) {
                ex_alu.perform(BusALU::op_rshift);
            }
            else if(two_o == 7) {
                ex_alu.perform(BusALU::op_rashift);
            }

            exmem.alu_out->latchFrom(ex_alu.OUT());
            return;
        }

        if(func_value == 24 && ir_type == 0) {//THIS IS THE SLT instruction
            //We compare rs(idex.a) with rt(idex.b) 
            if((*idex.a)(31,31) == 0 && (*idex.b)(31,31) == 1) {
                ex_alu.perform(BusALU::op_zero);
            }
            else if((*idex.a)(31,31) == 1 && (*idex.b)(31,31) == 0) {
                ex_alu.perform(BusALU::op_one);
            }
            else {
                if((*idex.a).value() >= (*idex.b).value()) {
                    ex_alu.perform(BusALU::op_zero);
                }
                else {    
                    ex_alu.perform(BusALU::op_one);
                }
            }
            exmem.alu_out->latchFrom(ex_alu.OUT());
            return;
        }
        if(ir_type == 24) {//THIS IS THE SLTI instruction
            //We compare rs(idex.a) with idex.imm, 
            if((*idex.a)(31,31) == 0 && (*idex.imm)(31,31) == 1) {
                ex_alu.perform(BusALU::op_zero);
            }
            else if((*idex.a)(31,31) == 1 && (*idex.imm)(31,31) == 0) {
                ex_alu.perform(BusALU::op_one);
            }
            else {
                if((*idex.a).value() >= (*idex.imm).value()) {
                    ex_alu.perform(BusALU::op_zero);
                }
                else {    
                    ex_alu.perform(BusALU::op_one);
                }
            }
            exmem.alu_out->latchFrom(ex_alu.OUT());
            return;
        }
        if(func_value == 25 && ir_type == 0) {//THIS IS THE SLTU instruction
            //We compare rs(idex.a) with rt(idex.b)
            if(  (*idex.a).value() < (*idex.b).value() ) {
                ex_alu.perform(BusALU::op_one);
            }
            else {
                ex_alu.perform(BusALU::op_zero);
            }
            exmem.alu_out->latchFrom(ex_alu.OUT());
            return;
        }

        if(ir_type == 39) {
            ex_alu.OP1().pullFrom(*idex.imm);
            ex_alu.OP2().pullFrom(*(shifty_boys[16]));
            exmem.alu_out->latchFrom(ex_alu.OUT());
            ex_alu.perform(BusALU::op_lshift);
            return;
        }

        ex_alu.OP1().pullFrom(*idex.a);

        long is_special = (*idex.ir)(31, 26);
        int func;

        if(is_special == 0) {//in this case is_special points out that we are preforming a R-R alu op
            ex_alu.OP2().pullFrom(*idex.b);
            func = (int)(*idex.ir)(2, 0);//this is the operation we are performing with ex_alu
        }
        else {
            ex_alu.OP2().pullFrom(*idex.imm);
            func = (int)(*idex.ir)(28, 26);
        }

        switch (func) {
            case 0:
                ex_alu.perform(BusALU::op_add);
                break;
            case 1:
                //unknown
                break;
            case 2:
                ex_alu.perform(BusALU::op_sub);
                break;
            case 3:
                //unknown
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
                //technically unknown
                //ex_alu.perform(BusALU::op_or);
                break;
        }

        exmem.alu_out->latchFrom(ex_alu.OUT());
        return;
   }

   if(ir_type >= 32 && ir_type < 48) {//32 -> 48 are all the load store operations we care about, we are preforming nothing from the special table
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
        return;
   }



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

void mem_stage_first_clock() {
    if(exmem.v->value() == 0) return;

    OPCodeClass opc = from_full_instruction_return_opcode_class(exmem.ir);
    int fake_op_dis = (*exmem.ir)(31, 26);

    if(opc == STORE) {
    /*
        Load/store instructions:
        MEM/WB.IR ← EX/MEM.IR;
    */

        mem_abus.IN().pullFrom(*exmem.imm);
        data_mem.MAR().latchFrom(mem_abus.OUT());
        data_mem.WRITE().pullFrom(*exmem.b);
    }
    if(opc == LOAD) {
    /*
        Load/store instructions:
        MEM/WB.IR ← EX/MEM.IR;
    */
        mem_abus.IN().pullFrom(*exmem.imm);
        data_mem.MAR().latchFrom(mem_abus.OUT());
    }
}

void mem_stage_second_clock() {
    if(exmem.v->value() == 0) return;
    mem_ir_thru.IN().pullFrom(*exmem.ir);
    memwb.ir->latchFrom(mem_ir_thru.OUT());
    OPCodeClass opc = from_full_instruction_return_opcode_class(exmem.ir);
    mem_v_thru.IN().pullFrom(*exmem.v);
    memwb.v->latchFrom(mem_v_thru.OUT());
    mem_pc_thru.IN().pullFrom(*exmem.pc);
    memwb.pc->latchFrom(mem_pc_thru.OUT());
    mem_npc_thru.IN().pullFrom(*exmem.npc);
    memwb.npc->latchFrom(mem_npc_thru.OUT());

    if(opc == ALU) {
        /*
        MEM/WB.IR ← EX/MEM.IR;
        MEM/WB.ALUOutput ← EX/MEM.ALUOutput;
        */

        mem_alu_out_thru.IN().pullFrom(*exmem.alu_out);
        memwb.alu_out->latchFrom(mem_alu_out_thru.OUT());
    }
    else if(opc == LOAD) {
        data_mem.read();
        memwb.lmd->latchFrom(data_mem.READ());
    }
    else if(opc == STORE) {
        data_mem.write();
    }
}


void wb_stage_first_clock() {

}

void wb_stage_second_clock() {
    if(memwb.v->value() == 0) return;
    OPCodeClass opc = from_full_instruction_return_opcode_class(memwb.ir);

    long ir_val = (*(memwb.ir))(31, 26);
    long pc_val = memwb.pc->value();
    long other_ir = (*(memwb.ir))(5, 0);
    const char *str = get_opcode_string_from_ir(memwb.ir); 
    cout << setfill('0') << setw(8) << pc_val << ":  " <<
    setfill('0') << setw(2) << ir_val << " ";
    if( ir_val == 0) {
        cout << setfill('0') << setw(2) << other_ir << " ";
    }
    else {
        cout << "   ";
    }

    cout << str;

    if(opc == ALU) {
        long is_special = (*memwb.ir)(31, 26);
        StorageObject* use_register;

        if(is_special == 0) {//all special alu opcode write into reg[Mem/WB.ir[rd]]
            size_t rd_index = (*memwb.ir)(15, 11);
            cout << " " << rnames[rd_index] << "[" 
              << setfill('0') << setw(8) << memwb.alu_out->value() << "]";
            use_register = reg_file[rd_index];
        }
        else {//they write into reg[Mem/WB.ir[rt]]
            size_t rt_index = (*memwb.ir)(20, 16);
            cout << " " << rnames[rt_index] << "["
              << setfill('0') << setw(8) << memwb.alu_out->value() << "]";
            use_register = reg_file[rt_index];
        }

        wb_bus.IN().pullFrom(*memwb.alu_out);
        use_register->latchFrom(wb_bus.OUT());
    }

    if(opc == LOAD) {
        size_t rt_index = (*memwb.ir)(20, 16);
        cout << " " << rnames[rt_index] << "["
          << setfill('0') << setw(8) << memwb.lmd->value() << "]";
        wb_bus.IN().pullFrom(*memwb.lmd);
        reg_file[rt_index]->latchFrom(wb_bus.OUT());
    }
    cout << '\n';
    if(ir_val == 0 && other_ir == 0) done = true;
    if(ir_val == 0 && other_ir == 7) {
        cout << "   ";
        int count = 0;
        for(int i = 0; i < 31; ++i) {
            if((reg_file[i])->value() != 0) {
                cout << "  " << *(reg_file[i]);
                ++count;
                if(!count%4) cout << '\n' << "   ";
            }
        }
        cout << '\n';
    }
}

void connect() {
    pc.connectsTo(addr_alu.OP2());
    pc.connectsTo(instr_abus.IN());
    pc.connectsTo(if_pc_thru.IN());
    pc.connectsTo(addr_alu.OUT());
    pc.connectsTo(branch_alu.OUT());
    pc.connectsTo(instr_mem.READ());
    instr_mem.MAR().connectsTo(instr_abus.OUT());
    const_addr_inc.connectsTo(addr_alu.OP1());
    const_valid_on.connectsTo(valid_bus.IN());
    const_valid_off.connectsTo(valid_bus.IN());

    ifid.v->connectsTo(valid_bus.OUT());
    ifid.v->connectsTo(id_v_thru.IN());
    ifid.ir->connectsTo(instr_mem.READ());
    ifid.ir->connectsTo(id_ir_thru.IN());
    ifid.ir->connectsTo(z_fill_imm_bus.IN());
    ifid.ir->connectsTo(sign_extend_alu.OP1());
    sign_extend_imm.connectsTo(sign_extend_alu.OUT());
    sign_extend_imm.connectsTo(branch_alu.OP1());
    sign_extend_imm.connectsTo(imm_bus.IN());
    ifid.pc->connectsTo(addr_alu.OUT());
    //  ifid.pc->connectsTo(branch_alu.OUT());
    ifid.pc->connectsTo(branch_alu.OP2());
    ifid.pc->connectsTo(id_pc_thru.IN());
    ifid.pc->connectsTo(if_pc_thru.OUT());
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
    idex.imm->connectsTo(ex_imm_thru.IN());
    idex.pc->connectsTo(id_pc_thru.OUT());
    idex.pc->connectsTo(ex_pc_thru.IN());
    idex.npc->connectsTo(id_npc_thru.OUT());
    idex.npc->connectsTo(ex_npc_thru.IN());
    idex.npc->connectsTo(ex_alu.OP1());
    idex.a->connectsTo(op1_bus.OUT());
    idex.a->connectsTo(ex_alu.OP1());
    idex.a->connectsTo(ex_alu.OP2());
    idex.a->connectsTo(ex_alu.OUT());
    idex.a->connectsTo(rs_lower5.IN());
    idex.b->connectsTo(op2_bus.OUT());
    idex.b->connectsTo(b_thru.IN());
    idex.b->connectsTo(ex_alu.OP2());
    idex.b->connectsTo(ex_alu.OP1());
    idex.imm->connectsTo(imm_bus.OUT());
    idex.imm->connectsTo(z_fill_imm_bus.OUT());
    idex.imm->connectsTo(ex_alu.OP1());
    idex.imm->connectsTo(ex_alu.OP2());
    shift_amt.connectsTo(ex_alu.OP2());
    shift_amt.connectsTo(rs_lower5.OUT());
    for(int i = 0; i < 32; ++i) {
        shifty_boys[i]->connectsTo(ex_alu.OP2());
    }

    //TODO COND?????
    exmem.v->connectsTo(ex_v_thru.OUT());
    exmem.v->connectsTo(mem_v_thru.IN());
    exmem.ir->connectsTo(ex_ir_thru.OUT());
    exmem.imm->connectsTo(ex_imm_thru.OUT());
    exmem.imm->connectsTo(mem_abus.IN());
    exmem.ir->connectsTo(mem_ir_thru.IN());
    exmem.pc->connectsTo(ex_pc_thru.OUT());
    exmem.pc->connectsTo(mem_pc_thru.IN());
    exmem.npc->connectsTo(ex_npc_thru.OUT());
    exmem.npc->connectsTo(mem_npc_thru.IN());
    exmem.b->connectsTo(b_thru.OUT());
    exmem.b->connectsTo(mem_abus.IN());
    exmem.b->connectsTo(data_mem.WRITE());
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

void simulate(char *objfile) {
    //CPUObject::debug |= CPUObject::trace;
    // Load object file

    instr_mem.load(objfile);
    data_mem.load(objfile);
    pc.latchFrom(instr_mem.READ());
    Clock::tick();
    while(!done) {
        wb_stage_first_clock();
        mem_stage_first_clock();
        ex_stage_first_clock();
        id_stage_first_clock();
        if_stage_first_clock();
        Clock::tick();
        wb_stage_second_clock();
        mem_stage_second_clock();
        ex_stage_second_clock();
        id_stage_second_clock();
        if_stage_second_clock();
        Clock::tick();
    }
}

int main(int argc, char *argv[]) {
  // Make sure we have an object file

    if(argc != 2) {
        cerr << "Usage " << argv[0] << " object-file\n\n";
        exit(1);
    }

    cout << hex; // Change base to hex for printing

    try {
        connect();
        simulate(argv[1]);
    }
    catch(ArchLibError &err){
        cout << endl
           << "Simulation aborted - ArchLib runtime error"
           << endl
           << "Cause: " 
           << err.what()
           << endl;
        return 1;
    }
    return 0;
};
