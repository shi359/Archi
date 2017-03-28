#include "instruction.h"
#include <stdlib.h>
#include <stdio.h>

void initial();
void end_pipe(int end);
void err_handle(int err_detect, int cycle);
void print_pipe();
void print_type(instruct* input);

FILE* output;
FILE* error;

void initial()
{
    output = fopen("snapshot.rpt", "w");
    error = fopen("error_dump.rpt", "w");
}

void print_mem(int cycle)
{
  int j = 0;

  fprintf(output,"cycle %d\n", cycle);

  for(j = 0; j < 32; j++)
      fprintf(output, "$%02d: 0x%08X\n", j, I_mem[j]);
  fprintf(output, "PC: 0x%08X\n", PC);

}


void print_pipe()
{
  
  // IF
  fprintf(output, "IF: ");
    if(IF == NULL)
        fprintf(output, "0x%08X", 0);
    else fprintf(output, "0x%08X", IF->code);
    
    if(stall != 0) fprintf(output, " to_be_stalled");
    if(jump != 0) fprintf(output, " to_be_flushed");
    fprintf(output, "\n");
  //ID
    
  fprintf(output, "ID: ");
  print_type(ID);
    
  if(stall != 0) fprintf(output, " to_be_stalled");
  if(id_fwd_EX_DM_rs_ != 0) fprintf(output, " fwd_EX-DM_rs_$%d", id_fwd_EX_DM_rs_);
  if(id_fwd_EX_DM_rt_ != 0) fprintf(output, " fwd_EX-DM_rt_$%d", id_fwd_EX_DM_rt_);
  fprintf(output, "\n");
  //EX

  fprintf(output, "EX: ");
  print_type(EX);
  if(fwd_DM_WB_rs_ != 0) fprintf(output, " fwd_DM-WB_rs_$%d", fwd_DM_WB_rs_);
  if(ex_fwd_EX_DM_rs_ != 0) fprintf(output, " fwd_EX-DM_rs_$%d", ex_fwd_EX_DM_rs_);
  if(fwd_DM_WB_rt_ != 0) fprintf(output, " fwd_DM-WB_rt_$%d", fwd_DM_WB_rt_);
  if(ex_fwd_EX_DM_rt_ != 0) fprintf(output, " fwd_EX-DM_rt_$%d", ex_fwd_EX_DM_rt_);
  
  
  fprintf(output, "\n");
  //DM
  fprintf(output, "DM: ");
  print_type(DM);
  fprintf(output, "\n");
  //WB 	 
  fprintf(output, "WB: ");
  print_type(WB);
  fprintf(output, "\n\n\n");
    
    

}

void print_type(instruct* input)
{
  if(input == NULL) fprintf(output, "NOP");
  else{
	  
  switch(input->op){
	case 0:
   	switch(input->func){
			case 0:
                if(input->rd == 0 && input->rt == 0 && input->shamt == 0)
                    fprintf(output, "NOP");
                else fprintf(output, "SLL");
            break;
			case 2: fprintf(output, "SRL"); break;
			case 3: fprintf(output, "SRA"); break;
			case 8: fprintf(output, "JR"); break;
			case 32: fprintf(output, "ADD"); break;
			case 34: fprintf(output, "SUB"); break;
			case 36: fprintf(output, "AND"); break;
			case 37: fprintf(output, "OR"); break;
			case 38: fprintf(output, "XOR"); break;
			case 39: fprintf(output, "NOR"); break;
			case 40: fprintf(output, "NAND"); break;
			case 42: fprintf(output, "SLT"); break;
		}
	break;
      case 2: fprintf(output, "J"); break;
      case 3: fprintf(output, "JAL"); break;
      case 4: fprintf(output, "BEQ"); break;
      case 5: fprintf(output, "BNE"); break;
      case 8: fprintf(output, "ADDI"); break;
      case 10: fprintf(output, "SLTI"); break;
      case 12: fprintf(output, "ANDI"); break;
      case 13: fprintf(output, "ORI"); break;
      case 14: fprintf(output, "NORI"); break;
      case 15: fprintf(output, "LUI"); break;
      case 32: fprintf(output, "LB"); break;
      case 33: fprintf(output, "LH"); break;
      case 35: fprintf(output, "LW"); break;
      case 36: fprintf(output, "LBU"); break;
      case 37: fprintf(output, "LHU"); break;
      case 40: fprintf(output, "SB"); break;
      case 41: fprintf(output, "SH"); break;
      case 43: fprintf(output, "SW"); break;
      case 63: fprintf(output, "HALT"); break;
    default: break;	
  }

 }
}



void print_err(int err_detect, int cycle)
{
    switch(err_detect){
        case 1:
            fprintf(error, "In cycle %d: Write $0 Error\n", cycle);
        break;
        
        case 2:
            fprintf(error, "In cycle %d: Address Overflow\n", cycle);
        break;
            
        case 3:
            fprintf(error, "In cycle %d: Misalignment Error\n", cycle);
        break;
            
        case 4:
            fprintf(error, "In cycle %d: Number Overflow\n", cycle);
        break;
            
        case 23:
            fprintf(error, "In cycle %d: Address Overflow\n", cycle);
            fprintf(error, "In cycle %d: Misalignment Error\n", cycle);
        
        break;
        
        default:
        break;
            
    }
    
}

void end_pipe(int end)
{
    
    if(end != 0){
        fclose(error);
        fclose(output);
        exit(1);
    }else return;
    
    
}
