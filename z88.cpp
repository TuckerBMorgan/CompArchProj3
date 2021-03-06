///
/// z88.cpp
///
/// T. Morgan. J.Schenk
///
/// This file contains most of the pieces that simulate the z88 processor
///
////////////////////////////////////////////////////////////////////////////////

#include <string.h>
#include <stdio.h>
#include <iostream>
#include "includes.h"
#include "globals.h"

const char *culprit;

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


//this function is used for getting the opcode class
//it is made so that it is easier and cleaner to handle different instructions in classes
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

//this just returns the opcode as a formatted string, used in printing for the most part
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
    //get the next instruction from memory
    instr_abus.IN().pullFrom(pc);
    instr_mem.MAR().latchFrom(instr_abus.OUT());
}

void if_stage_second_clock() {

    //all instructions have the opcode in the first 6 bits
    //we only need the top three to see what kinda of instruction it is
    long upper_opcode = (*ifid.ir)(31, 29);    //the * is required so we can deferance and get to the opcode
    long op = (*ifid.ir)(31, 26);

    long rs = (*ifid.ir)(25, 21);//IF/ID.ir(rs)
    long rt = (*ifid.ir)(20, 16);//IF/ID.ir(rt)
    long low_op = (*ifid.ir)(5,0);

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
    else if(op == 2 || op == 3 || (op == 0 && (low_op == 2 || low_op == 3))) {
        jump_pc_bus.IN().pullFrom(jump_reg);
        ifid.npc->latchFrom(jump_pc_bus.OUT());
        pc.latchFrom(jump_pc_bus.OUT());
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

    if(ifid.v->value() == 0) return;

    //setting up the sign extention used for branch and jump instructions 
    sign_extend_alu.OP1().pullFrom(*ifid.ir);
    sign_extend_alu.OP2().pullFrom(const_sign_extend_mask);
    sign_extend_alu.perform(BusALU::op_extendSign);
    sign_extend_imm.latchFrom(sign_extend_alu.OUT());

    //
    long op = (*ifid.ir)(31, 26);
    long low_op = (*ifid.ir)(5,0);

    //J and JAL
    if(op == 2 || op == 3) {
        
        jump_reg.latchFrom(jump_reg_thru.OUT());
        jump_reg_thru.IN().pullFrom(*ifid.ir);

        if(op == 3) {
            //setting up the mask, and then setting the PC
            branch_alu.OP1().pullFrom(const_eight);
            branch_alu.OP2().pullFrom(*ifid.pc);
            branch_alu.perform(BusALU::op_add);
            reg_file[31]->latchFrom(branch_alu.OUT());
        }
    }

    //JR and JALR
    if(op == 0 && (low_op == 2 || low_op == 3)) {
        

        //do we need to do jump forwarding
        long idex_rs = (*idex.ir)(25, 21);
        long memwb_rd = (*memwb.ir)(20, 16);
        long memwb_rt = (*memwb.ir)(15, 11);

        if(idex_rs == memwb_rd || idex_rs == memwb_rt) {
            jump_reg.latchFrom(jump_reg_thru32.OUT());
            jump_reg_thru32.IN().pullFrom(*memwb.alu_out);
        }
        else {
            jump_reg.latchFrom(jump_reg_thru32.OUT());
            jump_reg_thru32.IN().pullFrom(*reg_file[(*ifid.ir)(25, 21)]);
        }

        
        if(low_op == 3) {
            branch_alu.OP1().pullFrom(const_eight);
            branch_alu.OP2().pullFrom(*ifid.pc);
            branch_alu.perform(BusALU::op_add);
            reg_file[31]->latchFrom(branch_alu.OUT());
        }
    }
}

//mostly moving things along
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

//a function used in the deciscions to forward or not, takes care 
//of the left side of the table, amking sure we only forward on the correct source op code
bool should_forward() {
    bool ex_mem_is_r_r = false;
    bool mem_wb_is_r_r = false;
    bool ex_mem_is_alu_imm = false;
    bool mem_wb_is_alu_imm_or_load = false;

    long ex_mem_opcode = (*exmem.ir)(31, 26);
    long ex_mem_func = (*exmem.ir)(5, 0);
    if(ex_mem_opcode == 0 && (ex_mem_func >= 16 && ex_mem_func < 24)) {
        return true;
    }
    
    if(ex_mem_opcode >= 16 && ex_mem_opcode <= 24) {
        return true;
    }

    long mem_wb_opcode = (*memwb.ir)(31, 26);
    long mem_wb_func = (*memwb.ir)(5, 0);
    if(mem_wb_opcode == 0 && (mem_wb_func >= 16 && mem_wb_func < 24)) {
        return true;
    }

    if(mem_wb_opcode >= 16 && mem_wb_opcode <= 24) {
        return true;
    }
    
    if(mem_wb_opcode == 35) {
        return true;
    }
    return false;
}

void ex_stage_first_clock() {
    if(idex.v->value() == 0) return;
    long is_special = (*idex.ir)(31, 26);
    long is_nor = (*idex.ir)(5, 0);

    rs_lower5.IN().pullFrom(*(idex.a));
    shift_amt.latchFrom(rs_lower5.OUT());
}

void ex_stage_second_clock() { 
    if(idex.v->value() == 0) return;
	

    bool is_exmem_valid = exmem.v->value() == true;
    bool is_memwb_valid = memwb.v->value() == true;



    //--------------------------------------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------------------------------------
    //all the code below is used in the forwarding detetction
    //we do the initial tests up here, so all that code below can avoid having complicated tests all over
    bool ex_mem_destination_equals_id_ex_source = is_exmem_valid ? (*exmem.ir)(15, 11) == (*idex.ir)(25, 21) : false;
    bool ex_mem_destination_equals_id_ex_temp = is_exmem_valid ? (*exmem.ir)(15, 11) == (*idex.ir)(20, 16) : false;

    if( (*exmem.ir)(31, 26) != 0 && (*exmem.ir)(31, 26) == 35) {
        ex_mem_destination_equals_id_ex_source = false;
        ex_mem_destination_equals_id_ex_temp = false;
    }

    bool mem_wb_destination_equals_id_ex_source = is_memwb_valid ? (*memwb.ir)(15, 11) == (*idex.ir)(25, 21) : false;
    bool mem_wb_destination_equals_id_ex_temp = is_memwb_valid ? (*memwb.ir)(15, 11) == (*idex.ir)(20, 16) : false;

    if((*memwb.ir)(31, 26) != 0 && (*memwb.ir)(31, 26) == 35) {
        mem_wb_destination_equals_id_ex_source = false;
        mem_wb_destination_equals_id_ex_temp = false;
    }
        
    bool ex_mem_temp_equals_id_ex_source = is_exmem_valid ? (*exmem.ir)(20, 16) == (*idex.ir)(25, 21) : false;
    bool ex_mem_temp_equals_id_ex_temp = is_exmem_valid ? (*exmem.ir)(20, 16) == (*idex.ir)(20, 16) : false;

    bool mem_wb_temp_equals_id_ex_source = is_memwb_valid ? (*memwb.ir)(20, 16) == (*idex.ir)(25, 21) : false;
    bool mem_wb_temp_equals_id_ex_temp = is_memwb_valid ? (*memwb.ir)(20, 16) == (*idex.ir)(20, 16) : false;
    //------------------------------------------------------------------------------------------------------------
    //------------------------------------------------------------------------------------------------------------


    //only ALU and Load/Store opreations require we do this, but at the same time there is nore doing this for the branch insturctions as it doe not use the IR register
    ex_ir_thru.IN().pullFrom(*idex.ir);
    exmem.ir->latchFrom(ex_ir_thru.OUT());
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
    if( (ir_type  == 0 && func_value >= 16) || (ir_type >= 16 && ir_type < 32) || ir_type == 39) {
        long is_shifts = (*idex.ir)(5, 0);
        long two_o = (*idex.ir)(2, 0);
        long five_three = (*idex.ir)(5, 3);



        if(ir_type == 0 && ((is_shifts >= 37 && is_shifts <= 39) || (is_shifts >= 45 && is_shifts <= 47))){//ALLL SHIFT OPERATIONS

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
    
        //LUI
        if(ir_type == 39) {
            ex_alu.OP1().pullFrom(*idex.imm);
            ex_alu.OP2().pullFrom(*(shifty_boys[16]));
            exmem.alu_out->latchFrom(ex_alu.OUT());
            ex_alu.perform(BusALU::op_lshift);
            return;
        }

        ////forwarding code
        if(ex_mem_destination_equals_id_ex_source || mem_wb_destination_equals_id_ex_source 
            || ex_mem_temp_equals_id_ex_source || mem_wb_temp_equals_id_ex_temp) {
                if(ex_mem_destination_equals_id_ex_source || ex_mem_temp_equals_id_ex_source) {
                    if(should_forward()) { 
                        ex_alu.OP1().pullFrom(*exmem.alu_out);
                    }
                    else {
                        //top alu op
                        ex_alu.OP1().pullFrom(*idex.a);
                    }
                }
                else {
                    if(should_forward()) { 
                        ex_alu.OP1().pullFrom(*memwb.lmd);
                    }
                    else {
                        //top alu op
                        ex_alu.OP1().pullFrom(*idex.a);
                    }
                }
        
        }
        //if we preform no forwarding
        else {
            //top alu op
            ex_alu.OP1().pullFrom(*idex.a);
        }

        long is_special = (*idex.ir)(31, 26);
        int func;

        if(is_special == 0) {//in this case is_special points out that we are preforming a R-R alu op
        //forwarding
            if(ex_mem_destination_equals_id_ex_temp || ex_mem_temp_equals_id_ex_source || 
               mem_wb_destination_equals_id_ex_temp || mem_wb_temp_equals_id_ex_source || mem_wb_temp_equals_id_ex_temp) {

                if(ex_mem_destination_equals_id_ex_temp || ex_mem_temp_equals_id_ex_source) {
                    if(should_forward()) { 
                        ex_alu.OP2().pullFrom(*exmem.alu_out);
                    }
                    else {
                        ex_alu.OP2().pullFrom(*idex.b);
                    }

                }
                else {
                    if(should_forward()) { 
                        ex_alu.OP2().pullFrom(*memwb.lmd);
                    }
                    else {
                        ex_alu.OP2().pullFrom(*idex.b);
                    }
                }
            }
            //no forwarding
            else {
                ex_alu.OP2().pullFrom(*idex.b);
            }
            func = (int)(*idex.ir)(2, 0);//this is the operation we are performing with ex_alu
        }
        else {
            ex_alu.OP2().pullFrom(*idex.imm);
            func = (int)(*idex.ir)(28, 26);
            if(func == 18 || func == 19 || func == 23) return;
        }

        switch (func) {
            case 0:
                ex_alu.perform(BusALU::op_add);
                break;
            case 1:
                //unknown
                return;
            case 2:
                ex_alu.perform(BusALU::op_sub);
                break;
            case 3:
                //unknown
                return;
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
                return;
        }

        exmem.alu_out->latchFrom(ex_alu.OUT());
        return;
   }

   if(ir_type == 35 || ir_type == 43) {//32 -> 48 are all the load store operations we care about, we are preforming nothing from the special table

        //EX/MEM.IR ← ID/EX.IR;
        ex_ir_thru.IN().pullFrom(*idex.ir);
        exmem.ir->latchFrom(ex_ir_thru.OUT());

        //EX/MEM.B ← ID/EX.B;
        b_thru.IN().pullFrom(*idex.b);
        exmem.b->latchFrom(b_thru.OUT());

        if(ex_mem_destination_equals_id_ex_source || mem_wb_destination_equals_id_ex_source 
           || ex_mem_temp_equals_id_ex_source || mem_wb_temp_equals_id_ex_source) {
               if(ex_mem_destination_equals_id_ex_source || ex_mem_temp_equals_id_ex_source) {

                    if(should_forward()) { 
                        ex_alu.OP1().pullFrom(*exmem.alu_out);
                    }
                    else {
                        //EX/MEM.ALUOutput ← ID/EX.A + ID/EX.Imm;
                        ex_alu.OP1().pullFrom(*idex.a);
                    }
               }
               else {
                    if(should_forward()) { 
                        ex_alu.OP1().pullFrom(*memwb.alu_out);
                    }
                    else {
                        //EX/MEM.ALUOutput ← ID/EX.A + ID/EX.Imm;
                        ex_alu.OP1().pullFrom(*idex.a);
                    }
               }


           }
           else {
                //EX/MEM.ALUOutput ← ID/EX.A + ID/EX.Imm;
                ex_alu.OP1().pullFrom(*idex.a);
           }


        ex_alu.OP2().pullFrom(*idex.imm);
        ex_alu.perform(BusALU::op_add);
        exmem.alu_out->latchFrom(ex_alu.OUT());
        return;
   }


    if(ir_type == 60 || ir_type == 61) {
        //Branch

         if(ex_mem_destination_equals_id_ex_source || mem_wb_destination_equals_id_ex_source 
           || ex_mem_temp_equals_id_ex_source || mem_wb_temp_equals_id_ex_source) {
               if(ex_mem_destination_equals_id_ex_source || ex_mem_temp_equals_id_ex_source) {
                    if(should_forward()) { 
                        ex_alu.OP1().pullFrom(*exmem.alu_out);
                    } 
                    else {
                    // EXS/MEM.ALUOutput ← ID/EX.NPC + ID/EX.Imm;
                        ex_alu.OP1().pullFrom(*idex.npc);
                    }
               }
               else {
                   
                    if(should_forward()) { 
                        ex_alu.OP1().pullFrom(*memwb.alu_out);
                    } 
                    else {
                    // EXS/MEM.ALUOutput ← ID/EX.NPC + ID/EX.Imm;
                        ex_alu.OP1().pullFrom(*idex.npc);
                    }
               }
           }
        else {
        // EXS/MEM.ALUOutput ← ID/EX.NPC + ID/EX.Imm;
            ex_alu.OP1().pullFrom(*idex.npc);
        }


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
    if(exmem.v->value() == 0) return;
 
    OPCodeClass opc = from_full_instruction_return_opcode_class(exmem.ir);
    int fake_op_dis = (*exmem.ir)(31, 26);

    if(opc == STORE) {
    /*
        Load/store instructions:
        MEM/WB.IR ← EX/MEM.IR;
    */

        mem_abus.IN().pullFrom(*exmem.alu_out);
        data_mem.MAR().latchFrom(mem_abus.OUT());
        data_mem.WRITE().pullFrom(*exmem.b);
    }
    if(opc == LOAD) {
    /*
        Load/store instructions:
        MEM/WB.IR ← EX/MEM.IR;
    */
        mem_abus.IN().pullFrom(*exmem.alu_out);
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
    if(memwb.v->value() == 0) return;
    OPCodeClass opc = from_full_instruction_return_opcode_class(memwb.ir);

    long ir_val = (*(memwb.ir))(31, 26);
    long pc_val = memwb.pc->value();
    long other_ir = (*(memwb.ir))(5, 0);
    const char *str = get_opcode_string_from_ir(memwb.ir);

    if(strcmp(str, "-") == 0) {
        culprit = undef_instr;
        str = "???????";
        done = true;
    }
    if(strcmp(str, "ADDIU  ") == 0 ||
    strcmp(str, "LB     ") == 0 || strcmp(str, "LH     ") == 0 || 
    strcmp(str, "LBU    ") == 0 || strcmp(str, "LHU    ") == 0 ||
    strcmp(str, "SH     ") == 0 || strcmp(str, "SB     ") == 0 ||
    strcmp(str, "BLTZ   ") == 0 || strcmp(str, "BLTZAL ") == 0 ||
    strcmp(str, "BGEZ   ") == 0 || strcmp(str, "BGEZAL ") == 0 ||
    strcmp(str, "BLEZ   ") == 0 || strcmp(str, "BGTZ   ") == 0 ||
    strcmp(str, "SYSCALL") == 0 || strcmp(str, "ADDU   ") == 0 ||
    strcmp(str, "SUBU   ") == 0 || strcmp(str, "NOR    ") == 0) {
        culprit = unimp_instr;
        done = true;
    }

    cout << setfill('0') << setw(8) << pc_val << ":  " <<
    setfill('0') << setw(2) << ir_val << " ";
    if( ir_val == 0) {
        cout << setfill('0') << setw(2) << other_ir << " ";
    }
    else {
        cout << "   ";
    }

    cout << str;

    if(done) {
        cout << '\n';
        return;
    }

    if(ir_val == 3 || (ir_val == 0 && other_ir == 3)) {
        size_t rs_index = (*memwb.ir)(25, 21);
        cout << " " << rnames[rs_index] << "[" 
          << setfill('0') << setw(8) << reg_file[rs_index]->value() << "]";

    }
    if(opc == ALU) {
        long is_special = (*memwb.ir)(31, 26);
        StorageObject* use_register;

        if(is_special == 0) {//all special alu opcode write into reg[Mem/WB.ir[rd]]
            size_t rd_index = (*memwb.ir)(15, 11);

            if(rd_index != 0) {
                cout << " " << rnames[rd_index] << "[" 
                << setfill('0') << setw(8) << memwb.alu_out->value() << "]";
            }
            else {
                cout << " " << rnames[rd_index] << "[" 
                << setfill('0') << setw(8) << "00000000]";
            }

              if(rd_index != 0) {
                use_register = reg_file[rd_index];
              }
              else {
                  use_register = NULL;
              }
        }
        else {//they write into reg[Mem/WB.ir[rt]]
            size_t rt_index = (*memwb.ir)(20, 16);

            if(rt_index != 0) {
                cout << " " << rnames[rt_index] << "["
                << setfill('0') << setw(8) << memwb.alu_out->value() << "]";
            }
            else {
                cout << " " << rnames[rt_index] << "["
                << setfill('0') << setw(8) << "00000000]";
            }

              if(rt_index != 0) {
                use_register = reg_file[rt_index];
              }
              else {
                  use_register = NULL;
              }
        }

        wb_bus.IN().pullFrom(*memwb.alu_out);
        if(use_register != NULL) { 
            use_register->latchFrom(wb_bus.OUT());
        }
    }

    if(opc == LOAD) {
        size_t rt_index = (*memwb.ir)(20, 16);

        if(rt_index != 0) {
            cout << " " << rnames[rt_index] << "["
            << setfill('0') << setw(8) << memwb.lmd->value() << "]";
        }
        else {
            cout << " " << rnames[rt_index] << "["
            << setfill('0') << setw(8) << "00000000]";
        }
        if(rt_index != 0) {
            wb_bus.IN().pullFrom(*memwb.lmd);
            reg_file[rt_index]->latchFrom(wb_bus.OUT());
        }
    }
    cout << '\n';
    if(ir_val == 0 && other_ir == 0) {
        done = true;
        culprit = halt_instr;
    }
    if(ir_val == 0 && other_ir == 7) {
        cout << "   ";
        int count = 0;
        for(int i = 0; i < 31; ++i) {
            if((reg_file[i])->value() != 0) {
                cout << "  " << *(reg_file[i]);
                ++count;
                if(count%4 == 0) cout << '\n' << "   ";
            }
        }
        cout << '\n';
    }
}

void wb_stage_second_clock() {

}

///
/// Connects hardware components
///
void connect() {
    pc.connectsTo(addr_alu.OP2());
    pc.connectsTo(instr_abus.IN());
    pc.connectsTo(if_pc_thru.IN());
    pc.connectsTo(addr_alu.OUT());
    pc.connectsTo(branch_alu.OUT());
    pc.connectsTo(jump_pc_bus.OUT());
    pc.connectsTo(instr_mem.READ());
    instr_mem.MAR().connectsTo(instr_abus.OUT());
    const_addr_inc.connectsTo(addr_alu.OP1());
    const_valid_on.connectsTo(valid_bus.IN());
    const_valid_off.connectsTo(valid_bus.IN());
    const_nop_value.connectsTo(injection_bus.IN());



    ifid.v->connectsTo(valid_bus.OUT());
    ifid.v->connectsTo(id_v_thru.IN());
    ifid.ir->connectsTo(instr_mem.READ());
    ifid.ir->connectsTo(id_ir_thru.IN());
    ifid.ir->connectsTo(z_fill_imm_bus.IN());
    ifid.ir->connectsTo(sign_extend_alu.OP1());
    ifid.ir->connectsTo(jump_reg_thru.IN());
    jump_reg.connectsTo(jump_pc_bus.IN());
    jump_reg.connectsTo(jump_reg_thru.OUT());
    jump_reg.connectsTo(jump_reg_thru32.OUT());
    sign_extend_imm.connectsTo(sign_extend_alu.OUT());
    sign_extend_imm.connectsTo(branch_alu.OP1());
    sign_extend_imm.connectsTo(imm_bus.IN());
    const_eight.connectsTo(branch_alu.OP1());
    ifid.pc->connectsTo(addr_alu.OUT());
    //  ifid.pc->connectsTo(branch_alu.OUT());
    ifid.pc->connectsTo(branch_alu.OP2());
    ifid.pc->connectsTo(id_pc_thru.IN());
    ifid.pc->connectsTo(if_pc_thru.OUT());
    ifid.npc->connectsTo(addr_alu.OUT());
    ifid.npc->connectsTo(jump_pc_bus.OUT());
    ifid.npc->connectsTo(branch_alu.OUT());
    ifid.npc->connectsTo(branch_alu.OP2());
    ifid.npc->connectsTo(id_npc_thru.IN());
    const_sign_extend_mask.connectsTo(sign_extend_alu.OP2());
    reg_file[31]->connectsTo(branch_alu.OUT());
    for(int i = 0; i < 32; ++i) {
        reg_file[i]->connectsTo(op1_bus.IN());
        reg_file[i]->connectsTo(op2_bus.IN());
        reg_file[i]->connectsTo(wb_bus.OUT());
        reg_file[i]->connectsTo(jump_reg_thru32.IN());
    }

    idex.v->connectsTo(id_v_thru.OUT());
    idex.v->connectsTo(ex_v_thru.IN());
    idex.ir->connectsTo(id_ir_thru.OUT());
    idex.ir->connectsTo(ex_ir_thru.IN());
    idex.ir->connectsTo(injection_bus.OUT());
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

    exmem.v->connectsTo(ex_v_thru.OUT());
    exmem.ir->connectsTo(injection_bus.OUT());
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
    exmem.alu_out->connectsTo(ex_alu.OP1());
    exmem.alu_out->connectsTo(ex_alu.OP2());
    data_mem.MAR().connectsTo(mem_abus.OUT());

    memwb.v->connectsTo(mem_v_thru.OUT());
    memwb.ir->connectsTo(mem_ir_thru.OUT());
    memwb.pc->connectsTo(mem_pc_thru.OUT());
    memwb.npc->connectsTo(mem_npc_thru.OUT());
    memwb.alu_out->connectsTo(mem_alu_out_thru.OUT());
    memwb.alu_out->connectsTo(wb_bus.IN());
    memwb.alu_out->connectsTo(ex_alu.OP1());
    memwb.alu_out->connectsTo(ex_alu.OP2());
    memwb.lmd->connectsTo(data_mem.READ());
    memwb.lmd->connectsTo(wb_bus.IN());
    memwb.lmd->connectsTo(ex_alu.OP1());
    memwb.lmd->connectsTo(ex_alu.OP2());
}



//call this function before the first stage of the pipeline
//if it returns true, skip the current if stage and place a nop in its place
bool stall_check_for_idex() {
    
    if((*idex.ir)(31, 26) != 35) {
        return false;
    }

    //are the two registers ok, this avoids problems on unsaturated or stalled pipeline
    if(ifid.v->value() == true && idex.v->value() == true) {
           //if it is a special R-R ALU op

           
        if((*ifid.ir)(31, 26) == 0) {
            if((*ifid.ir)(5, 0) >= 16 && (*ifid.ir)(5, 0) <= 25) {
                if((*idex.ir)(20, 16) == (*ifid.ir)(25, 21)) {
                    return true;
                }        
            }
        }
        else if((*ifid.ir)(31, 26) >= 16) {
            if((*idex.ir)(20, 16) == (*ifid.ir)(20, 16) ) {
                return true;
            }
        }
    }
    return false;
}

bool stall_check_for_exmem() {

    if((*exmem.ir)(31, 26) != 35) {
        return false;
    }
    if(idex.v->value() == true && exmem.v->value() == true) {
        if((*ifid.ir)(31, 26) <= 50) {
            return false;
        }

        if((*ifid.ir)(25, 21) == (*exmem.ir)(20, 16)){
            return true;
        }
        else if((*ifid.ir)(25, 21) == (*exmem.ir)(20, 16)) {
            return true;
        }

    }
    return false;
}

//which_stage == true, inject nop into idex
//which_stage == false, ibject nop into exmem
void inject_nop_into_stage(bool which_stage) {
    injection_bus.IN().pullFrom(const_nop_value);
    if(which_stage) { 
        idex.ir->latchFrom(injection_bus.OUT());
    }
    else {
        exmem.ir->latchFrom(injection_bus.OUT());
    }
}

///
/// Runs z88 simulation
///
void simulate(char *objfile) {
  //  CPUObject::debug |= CPUObject::trace;
    // Load object file

    instr_mem.load(objfile);
    data_mem.load(objfile);
    pc.latchFrom(instr_mem.READ());
    Clock::tick();
    while(!done) {
        bool preform_idex_stall_load = stall_check_for_idex();
        bool preform_exmem_stall_load = stall_check_for_exmem();


        wb_stage_first_clock();
        mem_stage_first_clock();
        if(!preform_exmem_stall_load) {
            ex_stage_first_clock();
        }
        if(!preform_idex_stall_load && !preform_exmem_stall_load) {
            id_stage_first_clock();
            if_stage_first_clock();
        }
        Clock::tick();

        wb_stage_second_clock();
        mem_stage_second_clock();
        if(!preform_exmem_stall_load) {
            ex_stage_second_clock();
        }
        
        if(!preform_idex_stall_load && !preform_exmem_stall_load) {   
            id_stage_second_clock();
            if_stage_second_clock();
        }

        if(preform_idex_stall_load) {
            inject_nop_into_stage(true);
        }

        if(preform_exmem_stall_load) {
            inject_nop_into_stage(false);
        }
        
        Clock::tick();
    }
    cout << "Machine Halted - " << culprit;
}

///
/// Entry point
///
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
