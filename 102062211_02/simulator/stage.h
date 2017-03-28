#include<stdio.h>
#include<stdlib.h>
#include "instruction.h"
#include "error_handle.h"

int  do_wb();
int  do_dm();
int  do_ex();
void do_id();
int do_if();

void return_stall();
void ex_return_fwd(instruct* a, instruct* b);
void id_return_fwd();

int ex_result = 0;
int memory = 0;
int wb_ans = 0;
int ex_er = 0;
int dm_er = 0;
int wb_er = 0;

int jump = 0;
int to_jump = 0;
int pc_jal = 0;
int stall = 0;

int ex_fwd_EX_DM_rt_ = 0;
int ex_fwd_EX_DM_rs_ = 0;
int id_fwd_EX_DM_rt_ = 0;
int id_fwd_EX_DM_rs_ = 0;
int fwd_DM_WB_rt_ = 0;
int fwd_DM_WB_rs_ = 0;

int ex_fwd_RS = 0;
int ex_fwd_RT = 0;

instruct* IF;
instruct* ID;
instruct* EX;
instruct* DM;
instruct* WB;


int do_if(instruct* input)
{

    if(input != NULL)
        IF = input;
    
    else{
        IF= NULL;
        return 0;
    }
    
    if(IF->op == 0 && IF->rs == 0 && IF->func == 0 && IF->rt == 0 && IF->rd == 0 && IF->shamt == 0)
        IF = NULL;
    if(jump != 0)
        to_jump = 1;
    
    
    return stall;
}


void do_id()
{
    jump = 0;
    id_fwd_EX_DM_rt_ = id_fwd_EX_DM_rs_ = 0;
    
    int RS, RT;
    
    
    if(stall == 0){
        ID = IF;
        
        if(to_jump){
            ID = NULL;
            to_jump = 0;
        }
        if(ID == NULL) return;
        if(ID->op == 63) return;
        return_stall();
        if(stall != 0)
            return;
      }
    else{
       stall--;
       if(stall != 0) return;
    }
    id_return_fwd(ID, DM);
    
    if(id_fwd_EX_DM_rt_ != 0 || id_fwd_EX_DM_rs_ != 0){
        
        if(DM->op == 3){
            RS = (id_fwd_EX_DM_rs_ != 0)? pc_jal : I_mem[ID->rs];
            if(ID->op == 0 && ID->func == 8)
                RT = 0;
            else
                RT = (id_fwd_EX_DM_rt_ != 0)? pc_jal : I_mem[ID->rt];
        }
        else if(DM->op > 15){
            RS = (id_fwd_EX_DM_rs_ != 0)? memory : I_mem[ID->rs];
            if(ID->op == 0 && ID->func == 8)
                RT = 0;
            else
                RT = (id_fwd_EX_DM_rt_ != 0)? memory : I_mem[ID->rt];
        }
        else{
            RS = (id_fwd_EX_DM_rs_ != 0)? memory : I_mem[ID->rs];
            if(ID->op == 0 && ID->func == 8)
                RT = 0;
            else
                RT = (id_fwd_EX_DM_rt_ != 0)? memory : I_mem[ID->rt];
        }
    }
    else{
        if(ID->op == 15 || ID->op == 2 || ID->op == 3 ||
           (ID->op == 0 && (ID->func == 0 || ID->func == 2 || ID->func == 3)))
            RS = 0;
       
        else RS = I_mem[ID->rs];
        
        if((ID->op == 0 && ID->func == 8) || ID->op == 2 || ID->op == 3)
            RT = 0;
        else RT = I_mem[ID->rt];
    }
 
        if(ID->op == 2 || ID->op == 3){
            if(ID->op == 3)
                pc_jal = PC;
            PC = 4*ID->address;
            jump =  1;
        }
        else if(ID->op == 4 && RS == RT){
                PC += 4*ID->imm;
                jump =  1;
          }
        else if(ID->op == 5 && RS != RT){
                PC += 4*ID->imm;
                jump =  1;
        }
        else if(ID->op == 0 && ID->func == 8){
            PC = RS;
            jump = 1;
        }
    
}


int do_ex()
{
    ex_er = 0;
    ex_fwd_EX_DM_rt_ = ex_fwd_EX_DM_rs_ = 0;
    fwd_DM_WB_rt_ = fwd_DM_WB_rs_ = 0;
    
    if(stall != 0){
        EX = NULL;
        return 0;
    }
    
    EX = ID;
    
    if(EX == NULL) return 0;
    if(EX->op == 63 || EX->op == 2 || EX->op == 3
       || EX->op == 4 || EX->op == 5 || (EX->op == 0 && EX->func == 8))
        return 0;
    ex_return_fwd(EX, WB);
    ex_return_fwd(EX, DM);
    
    if(fwd_DM_WB_rs_ == ex_fwd_EX_DM_rs_ && fwd_DM_WB_rt_ == ex_fwd_EX_DM_rt_)
        fwd_DM_WB_rs_ = fwd_DM_WB_rt_ = 0;
    
    int RS, RT;
   
    if(fwd_DM_WB_rs_ != 0 || fwd_DM_WB_rt_ != 0){
        RS = (fwd_DM_WB_rs_ == 0)? I_mem[EX->rs] : wb_ans;
        RT = (fwd_DM_WB_rt_ == 0)? I_mem[EX->rt] : wb_ans;
    }
    if(ex_fwd_EX_DM_rs_ != 0 || ex_fwd_EX_DM_rt_ != 0){
        
        if(DM->op == 3){
            if(EX->op == 15 || EX->op == 2 || EX->op == 3 ||
               (EX->op == 0 && (EX->func == 0 || EX->func == 2 || EX->func == 3))){
                RS = 0;
            }
            else
                RS = (ex_fwd_EX_DM_rs_ == 0)? I_mem[EX->rs] : pc_jal;
            RT = (ex_fwd_EX_DM_rt_ == 0)? I_mem[EX->rt] : pc_jal;
        }
        else if(DM->op > 15){
            if(EX->op == 15 || EX->op == 2 || EX->op == 3 ||
               (EX->op == 0 && (EX->func == 0 || EX->func == 2 || EX->func == 3))){
                RS = 0;
            }
            else
                RS = (ex_fwd_EX_DM_rs_ == 0)? I_mem[EX->rs] : memory;
            RT = (ex_fwd_EX_DM_rt_ == 0)? I_mem[EX->rt] : memory;
        }
        else{
            if(EX->op == 15 || EX->op == 2 || EX->op == 3 ||
               (EX->op == 0 && (EX->func == 0 || EX->func == 2 || EX->func == 3))){
                RS = 0;
            }
            else
                RS = (ex_fwd_EX_DM_rs_ == 0)? I_mem[EX->rs] : ex_result;
            RT = (ex_fwd_EX_DM_rt_ == 0)? I_mem[EX->rt] : ex_result;
            
        }
    }
    else{
        if(EX->op == 15 || EX->op == 2 || EX->op == 3 ||
           (EX->op == 0 && (EX->func == 0 || EX->func == 2 || EX->func == 3))){
            RS = 0;
        }
        else RS = I_mem[EX->rs];
        RT = I_mem[EX->rt];
    }

    int i;
    if(EX->op == 0){
        switch(EX->func){
            case 0: //SLL
                ex_result = RT;
                ex_result = ex_result << EX->shamt;
            break;
            case 2: //SRL
                ex_result = RT;
                ex_result = (unsigned int)ex_result >> EX->shamt;
            break;
            case 3: //SRA
                ex_result = RT >> EX->shamt;
                break;
            case 32: //ADD
                ex_result = RS + RT;
                ex_er = no_add(RS, RT, ex_result);
                break;
            case 34: //SUB
                ex_result = RS + (-1)*RT;
                ex_er = no_sub(RS, RT, ex_result);
                break;
            case 36: //AND
                ex_result = RS & RT;
                break;
            case 37: //OR
                ex_result = RS | RT;
            break;
            case 38: //XOR
                ex_result = RS ^ RT;
                break;
            case 39: //NOR
                ex_result = ~(RS | RT);
                break;
            case 40: //NAND
                ex_result = ~(RS & RT);
                break;
            case 42: //SLT
                ex_result = (RS < RT)? 1 : 0;
                break;
        }
        

    }
    else{
        switch(EX->op){
            case 8: //ADDI
                ex_result = RS + EX->imm;
                ex_er = no_add(RS, EX->imm, ex_result);
                break;
                
            case 10: //SLTI
                ex_result = (RS < EX->imm)? 1 : 0;
                break;
                
            case 12: //ANDI
                ex_result = EX->imm;
                if(ex_result >> 31 != 0) ex_result = ex_result & 0x0000FFFF;
                ex_result = RS & ex_result;
                break;
                
            case 13: //ORI
                ex_result = EX->imm;
                if(ex_result >> 31 != 0) ex_result = ex_result & 0x0000FFFF;
                ex_result = RS | ex_result;
                break;
                
            case 14: //NORI
                ex_result =EX->imm;
                if(ex_result >> 31 != 0) ex_result = ex_result & 0x0000FFFF;
                ex_result = ~(RS | ex_result);
                break;
                
            case 15: //LUI
                ex_result = EX->imm << 16;
                break;
                
            case 32:
                ex_result = RS + EX->imm; //LB
                ex_er = no_I(RS, EX->imm, ex_result);
                break;
                
            case 33:
                ex_result = RS + EX->imm; //LH
                ex_er = no_I(RS, EX->imm, ex_result);
                break;
                
            case 35:
                ex_result = RS + EX->imm; //LW
                ex_er = no_I(RS, EX->imm, ex_result);
                break;
                
            case 36:
                ex_result = RS + EX->imm; //LBU
                ex_er = no_I(RS, EX->imm, ex_result);
                break;
                
            case 37:
                ex_result = RS + EX->imm; //LHU
                ex_er = no_I(RS, EX->imm, ex_result);
                break;
                
            case 40:
                ex_result = RS + EX->imm; //SB
                ex_er = no_I(RS, EX->imm, ex_result);
                break;
                
            case 41:
                ex_result = RS + EX->imm; //SH
                ex_er = no_I(RS, EX->imm, ex_result);
                break;
                
            case 43:
                ex_result = RS + EX->imm; //SW
                ex_er = no_I(RS, EX->imm, ex_result);
                break;
                //FF
            case 63:
                break;
                
            default:
                break;
                
        }

    }
    return ex_er;
}
int do_dm()
{
    DM = EX;
    
    if(DM == NULL) return 0;
    if(DM->op == 63) return 0;

    dm_er = 0;
    memory = 0;
    
    switch(DM->op){

			case 32: //LB
                if(ex_result + 1 > 1024 || ex_result + 1 < 0 || ex_result < 0) dm_er = 2;
                else{
                    int b = D_mem[ex_result/4] << 8*(ex_result%4);
                    memory = b >> 8*3;
                }
			break;

			case 33: //LH
                if(ex_result + 2 > 1024 || ex_result + 2 < 0 || ex_result < 0) dm_er = 2;
                if(ex_result % 2 != 0) dm_er = dm_er*10 + 3;
                if(dm_er == 0){
                    int a = D_mem[ex_result/4];
                    if(ex_result % 4 == 0)
                        memory = (a & 0xFFFF0000) >> 16;
                    else if(ex_result % 4 == 2)
                        memory  = D_mem[ex_result/4] & 0x0000FFFF;
                    if(memory  >> 15 == 1) memory = memory | 0xFFFF0000;
                }
            break;

			case 35: //LW
                if(ex_result + 4 > 1024 || ex_result + 4 < 0 || ex_result < 0) dm_er = 2;
            if(ex_result % 4 != 0) dm_er = dm_er*10 + 3;
                if(dm_er == 0)
				  memory = D_mem[ex_result/4];
			break;

			case 36: //LBU
                if(ex_result + 1 > 1024 || ex_result + 1 < 0 || ex_result < 0) dm_er = 2;
                else{
                    int b = D_mem[ex_result/4];
                    b = b << 8*(ex_result%4);
                    int c = b >> 8*3;
                    if(c >> 7 != 0) c = c & 0x000000FF;
                    memory = c;
                }
                break;
			
			case 37: //LHU
                if(ex_result + 2 > 1024 || ex_result + 2 < 0 || ex_result < 0) dm_er = 2;
                if(ex_result % 2 != 0) dm_er = dm_er*10 + 3;
                if(dm_er == 0){
                    int a = D_mem[ex_result/4];
                    if(ex_result % 4 == 0){
                        a = (a & 0xFFFF0000) >> 16;
                        if(a >> 15 != 0) a = a & 0x0000FFFF;
                        memory = a;
                    }
                    else if(ex_result % 4 == 2){
                        a = a & 0x0000FFFF;
                        memory = a;
                    }
    
                }
            break;
                
            case 40: //SB
                if(ex_result + 1  > 1024 || ex_result + 1 < 0 || ex_result < 0)
                    dm_er = 2;
                if(dm_er == 0){
                    int a = I_mem[DM->rt] & 0x000000FF;
                    switch(ex_result % 4){
                        case 0:
                            D_mem[ex_result/4] =  (a << 24) + (D_mem[ex_result/4] & 0x00FFFFFF);
                        break;
                        case 1:
                            D_mem[ex_result/4] = (a << 16) + (D_mem[ex_result/4] & 0xFF00FFFF);
                        break;
                        case 2:
                            D_mem[ex_result/4] = (a << 8) + (D_mem[ex_result/4] & 0xFFFF00FF);
                        break;
                        case 3:
                            D_mem[ex_result/4] = a + (D_mem[ex_result/4] & 0xFFFFFF00);
                        break;
                        memory = D_mem[ex_result/4];
                    }
            break;
                    
            case 41: //SH
                if(ex_result + 2  > 1024 || ex_result + 2 < 0 || ex_result < 0)
                    dm_er = 2;
                if(ex_result % 2 != 0) dm_er =  dm_er*10 + 3;
                if(dm_er == 0){
                    int a  = I_mem[DM->rt] & 0x0000FFFF;
                    if(ex_result % 4 == 0) D_mem[ex_result/4] = (a << 16) + (D_mem[ex_result/4] & 0x0000FFFF);
                    else if(ex_result % 2 == 0) D_mem[ex_result/4] = a + (D_mem[ex_result/4] & 0xFFFF0000);
                    memory = D_mem[ex_result/4];
                 }
            break;
            case 43: //SW
                if(ex_result + 4  > 1024 || ex_result + 4 < 0 || ex_result < 0)
                    dm_er = 2;
                if(ex_result % 4 != 0) dm_er =  dm_er*10 + 3;
                if(dm_er == 0) D_mem[ex_result/4] = I_mem[DM->rt];
                    memory = D_mem[ex_result/4];
            break;
			
            default:
                dm_er = 0;
                memory = ex_result;
            break;
		}
    }
    
    return dm_er;
}
int  do_wb()
{
    
  WB = DM;
  if(WB == NULL) return 0;
  if(WB->op == 63) return 0;
  wb_er = 0;
    
    if(WB->op == 0){
        if(WB->rd == 0){
            if(WB->func == 0){
                if(WB->rt != 0 || WB->shamt != 0)
                    wb_er = 1;
            }
            else if(WB->func != 8)
                wb_er = 1;
            
        }
        else if(WB->func != 8){
            wb_ans = memory;
            I_mem[WB->rd] = wb_ans;
        }
    }
    else if(WB->op == 3){
        I_mem[31] = pc_jal;
        wb_ans = pc_jal;
        pc_jal = 0;
    }
    else if(WB->op == 8 || WB->op == 10 || WB->op == 12 || WB->op == 13 || WB->op == 14 || WB->op == 15){
        wb_er = (WB->rt == 0)? 1 : 0;
        if(wb_er == 0){
            wb_ans = memory;
            I_mem[WB->rt] = wb_ans;
        }
        
    }
    else if(WB->op == 32 || WB->op == 33 || WB->op == 35 || WB->op == 36 || WB->op == 37){
        wb_er = (WB->rt == 0)? 1 : 0;
        if(wb_er == 0 && dm_er == 0){
            wb_ans = memory;
            I_mem[WB->rt] = wb_ans;
        }
        
    }
    
    return wb_er;
}

            
void return_stall()
{
    stall = 0;
    // conditional branch
    if(ID->op == 4 || ID->op == 5){
        if(EX != NULL){
            if(EX->op == 0){
                if(EX->func != 8 && EX->rd != 0){
                    if(EX->rd == ID->rs || EX->rd == ID->rt) stall = 1;
                }
            }
            else if(EX->op == 32 || EX->op == 33 || EX->op == 35 || EX->op == 36 || EX->op == 37){
                if((EX->rt == ID->rs || EX->rt == ID->rt) && EX->rt != 0)
                    stall = 2;
            }
            else if(EX->op == 8 || EX->op == 10 || EX->op == 12
                    || EX->op == 13 || EX->op == 14 || EX->op == 15){
                if((EX->rt == ID->rs || EX->rt == ID->rt) && EX->rt != 0)
                    stall = 1;
            }
        }
        
        if(DM != NULL && stall == 0){
           if(DM->op == 32 || DM->op == 33 || DM->op == 35 || DM->op == 36 || DM->op == 37){
            if((DM->rt == ID->rs || DM->rt == ID->rt) && DM->rt != 0)
                stall = 1;
           }
        }

    }
    
    // unconditional branch
    else if(ID->op == 0 && ID->func == 8){
        if(EX != NULL){
            if(EX->op == 0){
                if(EX->func != 8 && EX->rd != 0)
                    if(EX->rd == ID->rs) stall = 1;
            }
            else if(EX->op == 8 || EX->op == 10 || EX->op == 12 ||
                    EX->op == 13 || EX->op == 14 || EX->op == 15){
                if(EX->rt == ID->rs && EX->rt != 0)
                    stall = 1;
            }
            else if(EX->op == 32 || EX->op == 33 || EX->op == 35 || EX->op == 36 || EX->op == 37){
                if(EX->rt == ID->rs && EX->rt != 0)
                    stall = 2;
            }
            
        }
        
        if(DM != NULL && stall == 0){
            if(DM->op == 32 || DM->op == 33 || DM->op == 35 || DM->op == 36 || DM->op == 37){
                if(DM->rt == ID->rs && DM->rt != 0)
                    stall = 1;
            }
            
        }
        
        
    }
 
    // R-type store
    else if(ID->op == 0 || ID->op == 40 || ID->op == 41 || ID->op == 43){
        
        if(EX != NULL){
            if(EX->op == 32 || EX->op == 33 || EX->op == 35 || EX->op == 36 || EX->op == 37){
                
                if(ID-> op == 0 && (ID->func == 0 || ID->func == 2 || ID->func == 3)){
                    if(EX->rt == ID->rt && EX->rt != 0) stall = 1;
                }
                else if((EX->rt == ID->rs || EX->rt == ID->rt) && EX->rt != 0)
                        stall = 1;
            }
        }/*
        if(DM != NULL){
            if(DM->op == 32 || DM->op == 33 || DM->op == 35 || DM->op == 36 || DM->op == 37){
                if((DM->rt == ID->rs || DM->rt == ID->rt) && DM->rt != 0)
                    stall = 1;
            }
        }*/
    }
    
    // I-type
    else if(ID->op == 8 || ID->op == 10 || ID->op == 12 || ID->op == 13 || ID->op == 14 ||
            ID->op == 15 || ID->op == 32 ||ID->op == 33 || ID->op == 35 || ID->op == 36 ||
            ID->op == 37)
    {
        if(EX != NULL){
            if(EX->op == 32 || EX->op == 33 || EX->op == 35 || EX->op == 36 || EX->op == 37){
                if(EX->rt == ID->rs && EX->rt != 0) stall = 1;
            }
        }
        /*if(DM != NULL && stall == 0){
            if(DM->op == 32 || DM->op == 33 || DM->op == 35 || DM->op == 36 || DM->op == 37){
                if(ID->op != 15 && DM->rt == ID->rs && DM->rt != 0) stall = 1;
            }
        }*/
    }

}

void ex_return_fwd(instruct* a, instruct* b)
{
    
    if(a == NULL || b == NULL || stall > 1) return;
    if(a->op == 63) return;
   
    if(b->op == 0 && b->func != 8){
        
        if((a->op == 0 && a->func != 8) || a->op == 40 || a->op == 41 || a->op == 43){
            if(b->rd == a->rt){
                if(b == WB) fwd_DM_WB_rt_ = b->rd;
                else ex_fwd_EX_DM_rt_ = b->rd;
            }
            if(b->rd == a->rs){
                if(b == WB) fwd_DM_WB_rs_ = b->rd;
                else ex_fwd_EX_DM_rs_ = b->rd;
                if(a->op == 0 && (a->func == 0 || a->func == 2 || a->func == 3))
                    fwd_DM_WB_rs_ = ex_fwd_EX_DM_rs_ = 0;
            }
        }

        else if(a->op == 8 || a->op == 10 || a->op == 12 || a->op == 13 || a->op == 14 ||
               a->op == 32 ||a->op == 33 || a->op == 35 || a->op == 36 || a->op == 37 ){
                if(b->rd == a->rs){
                    if(b == WB) fwd_DM_WB_rs_ = b->rd;
                    else ex_fwd_EX_DM_rs_ = b->rd;
                }
        }
    }
    
    else if(b->op == 8 || b->op == 10 || b->op == 12 || b->op == 13 || b->op == 14 ||
            b->op == 15 || b->op == 32 ||b->op == 33 || b->op == 35 || b->op == 36 || b->op == 37)
    {
        
        if((a->op == 0 && a->func != 8) || a->op == 40 || a->op == 41 || a->op == 43){
            if(b->rt == a->rt){
                if(b == WB) fwd_DM_WB_rt_ = b->rt;
                else ex_fwd_EX_DM_rt_ = b->rt;
             }
             if(b->rt == a->rs){
                if(b == WB) fwd_DM_WB_rs_ = b->rt;
                else ex_fwd_EX_DM_rs_ = b->rt;
                 
                 if(a->op == 0 && (a->func == 0 || a->func == 2 || a->func == 3))
                     fwd_DM_WB_rs_ = ex_fwd_EX_DM_rs_ = 0;
            }
        }
        
        else if(a->op == 8 || a->op == 10 || a->op == 12 || a->op == 13 || a->op == 14 ||
                a->op == 32 ||a->op == 33 || a->op == 35 || a->op == 36 || a->op == 37 ){
            if(b->rt == a->rs){
                if(b == WB) fwd_DM_WB_rs_ = b->rt;
                else ex_fwd_EX_DM_rs_ = b->rt;
            }
        }
    }
    else if(b->op == 3){
        if((a->op == 0 && a->func != 8) || a->op == 40 || a->op == 41 || a->op == 43){
            if(a->rt == 31){
                if(b == WB) fwd_DM_WB_rt_ = 31;
                else ex_fwd_EX_DM_rt_ = 31;
            }
            if(a->rs == 31){
                if(b == WB) fwd_DM_WB_rs_ = 31;
                else ex_fwd_EX_DM_rs_ = 31;
                if(a->op == 0 && (a->func == 0 || a->func == 2 || a->func == 3))
                    fwd_DM_WB_rs_ = ex_fwd_EX_DM_rs_ = 0;
            }
        }
        else if(a->op == 8 || a->op == 10 || a->op == 12 || a->op == 13 || a->op == 14 ||
                a->op == 32 ||a->op == 33 || a->op == 35 || a->op == 36 || a->op == 37 ){
            if(a->rs == 31){
                if(b == WB) fwd_DM_WB_rs_ = 31;
                else ex_fwd_EX_DM_rs_ = 31;
            }
        }
    }

}

void id_return_fwd(instruct* a, instruct* b)
{
    if(ID == NULL || DM == NULL || stall > 1) return;
    if(ID->op == 63) return;

    if(DM->op == 0){
        if(ID->op == 4 || ID->op == 5){
                if(DM->rd == ID->rt && DM->rd != 0) id_fwd_EX_DM_rt_ = DM->rd;
                if (DM->rd == ID->rs && DM->rd != 0) id_fwd_EX_DM_rs_ = DM->rd;
        }
        else if(ID->op == 0 && ID->func == 8){
            if(DM->rd == ID->rs) id_fwd_EX_DM_rs_ = DM->rd;
        }
    }
    else if(DM->op == 8 || DM->op == 10 || DM->op == 12 || DM->op == 13 || DM->op == 14 ||
            DM->op == 15)
    {
        if(ID->op == 4 || ID->op == 5){
            if(DM->rt == ID->rt && DM->rt != 0) id_fwd_EX_DM_rt_ = DM->rt;
            if (DM->rt == ID->rs && DM->rt != 0) id_fwd_EX_DM_rs_ = DM->rt;
        }
        else if(ID->op == 0 && ID->func == 8){
            if(DM->rt == ID->rs) id_fwd_EX_DM_rs_ = DM->rt;
        }
    }
    else if(DM->op == 3){
        if(ID->op == 4 || ID->op == 5){
            if(ID->rs == 31) id_fwd_EX_DM_rs_ = 31;
            if(ID->rt == 31) id_fwd_EX_DM_rt_ = 31;
        }
        else if(ID->op == 0 && ID->func == 8){
            if(ID->rs == 31) id_fwd_EX_DM_rs_ = 31;
        }
    }
}
