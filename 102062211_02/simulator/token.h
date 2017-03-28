#include <stdlib.h>
#include "instruction.h"
instruct token(instruct *input, unsigned int tmp);

instruct token(instruct *input, unsigned int tmp)
{
     input->op = (tmp >> 26);
     if(input->op == 0)
     {
       input->rs = (tmp >> 21)&31;
       input->rd = (tmp >> 11)&31;
       input->rt = (tmp >> 16)&31;
       input->shamt = (tmp >> 6)&31;
       input->func = tmp & 63;
     }

     else if(input->op == 2 || input->op == 3)
       input->address = (tmp << 6) >> 6;
     else
     {
        input->imm = tmp & 65535;
         if(input->imm >> 15 == 1) input->imm  = input->imm | 0xFFFF0000;
        input->rt = (tmp >> 16)&31;
        input->rs = (tmp >> 21)&31;

     }
     input->code = tmp;	
     return *input;
}

