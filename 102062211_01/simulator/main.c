#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "instruction.h"
#include "token.h"
#include "R_type.h"
#include "I_type.h"


void do_func(instruct* input);
void traverse(instruct* input);
void print_mem(int err_detect, int cycle);
void err_handle(int err_detect, int cycle);
void read_iimage();
void read_dimage();
unsigned int swap(unsigned int);

FILE* iimage;
FILE* bimage;
FILE* output;
FILE* error;

instruct* head;
instruct* flag;

unsigned int fp = 0;
unsigned int ra = 0;

int main(int argc, char *argv[])
{

   iimage = fopen("iimage.bin", "rb");
   bimage = fopen("dimage.bin", "rb");
   output = fopen("snapshot.rpt", "w");
   error = fopen("error_dump.rpt", "w");

   memset(I_mem, 0, sizeof(I_mem));
   memset(D_mem, 0, sizeof(D_mem));

   read_iimage();
   read_dimage();
   print_mem(0,0);
   traverse(head);

   fclose(output);

  return 0;
}

void traverse(instruct* input)
{
   instruct *it;
   it = input;


   while(it != NULL && it->op != 63)
   {
       do_func(it);

     if(it->nxt == NULL)
        break;
     else it = it->nxt;
   }
   fclose(output);
   exit(1);



}

void read_iimage()
{
   int i = 0;
   int i_count = 0;
   int read_pc = 0;
   unsigned int tmp = 0;
   instruct *input;
   instruct *it;

   if(iimage == NULL){
		fprintf(stderr, "[ERROR] OPEN iimage\n");
		return ;
	}

    for(i = 0; !feof(iimage); i++){

     if(i != 0 && i > i_count+1) break;
     fread(&tmp, sizeof(unsigned int), 1, iimage);
     // little-endian to big-endian
     tmp = swap(tmp);

     // first four bytes
     if(i == 0){
       PC = tmp;
       read_pc = tmp;
       continue;
     }

     // second four bytes
     else if(i == 1){
         i_count = tmp;
         continue;
     }

     read_pc += 4;

     input = malloc(sizeof(instruct));

     *input = token(input, tmp);
     input->nxt = NULL;
     input->pre = NULL;
     input->label = read_pc;

     if(i == 2)
        head = input;
     else{
        it = head;
       while(it != NULL){
            if(it->nxt == NULL){
               input->pre = it;
               it->nxt = input;
               break;
            }
            else it = it->nxt;
     }
   }

  }
}

void read_dimage()
{
  int i = 0;
  unsigned int tmp = 0;
  if(bimage == NULL){
		fprintf(stderr, "[ERROR] OPEN dimage\n");
		return;
	}

  for(i = 0; !feof(bimage); i++){
    if(i != 0 && i > b_count+1) break;
    fread(&tmp, sizeof(unsigned int), 1, bimage);
    tmp = swap(tmp);
    if(i == 0) {
       SP = tmp;
       continue;
    }
    if(i == 1){
        b_count = tmp;
        continue;
    }

    D_mem[i-2] = tmp;
  }
  I_mem[29] = SP;

}

void do_func(instruct* input)
{
    static int cycle = 1;
    instruct* it;
    int jump = 0;
    int j_return = 0;
    int err_detect = 0;

    //printf("label = 0x%04X\n", input->label);

    switch(input->op){
        case 0:
            switch(input->func){
                case 0:
                    //printf("SLL\n");
                    PC += 4;
                    err_detect = SLL(input);
                break;

                case 2:
                    //printf("SRL\n");
                    PC += 4;
                    err_detect = SRL(input);
                break;

                case 3:
                    //printf("SRA\n");
                    PC += 4;
                    err_detect = SRA(input);
                break;

                case 8:
                    //printf("JR\n");
                    PC = I_mem[31];
                    err_detect = 0;
                    j_return = 1;
                    it = head;
                    while(it != NULL){
                        if(it->label == PC) break;
                        if(it->nxt != NULL) it = it->nxt;
                        else break;
                    }
                    break;

                case 32:
                    //printf("ADD\n");
                    PC += 4;
                    err_detect = ADD(input);
                break;

                case 34:
                    //printf("SUB\n");
                    PC += 4;
                    err_detect = SUB(input);
                break;

                case 36:
                    //printf("AND\n");
                    PC += 4;
                    err_detect = AND(input);
                break;

                case 37:
                    //printf("OR\n");
                    PC += 4;
                    err_detect = OR(input);
                break;

                case 38:
                    //printf("XOR\n");
                    PC += 4;
                    err_detect = XOR(input);
                break;

                case 39:
                    //printf("NOR\n");
                    PC += 4;
                    err_detect = NOR(input);
                break;

                case 40:
                    //printf("NAND\n");
                    PC += 4;
                    err_detect = NAND(input);
                break;

                case 42:
                    //printf("SLT\n");
                    PC += 4;
                    err_detect = SLT(input);
                break;

            }
        break;

        case 2: //j
            //printf("J\n");
            PC = input->address*4;
            it = head;
            while(it != NULL){
              if(it->label == input->address*4) break;
              if(it->nxt != NULL) it = it->nxt;
              else break;
            }
            jump = 1;
        break;

        case 3: //jal

            I_mem[31] = PC + 4;
            PC = input->address*4;
            it = head;
            while(it != NULL){
              if(it->label == input->address*4) break;
              if(it->nxt != NULL) it = it->nxt;
              else break;
            }
            jump = 1;
        break;

        case 4: // BEQ
            //printf("BEQ\n");
            if(I_mem[input->rs] == I_mem[input->rt])
            {
                int tmp = input->imm;
                if(input->imm >> 15 == 1) tmp = input->imm | 0xFFFF0000;
                PC = PC + 4 + tmp*4;
                it = head;
                while(it != NULL){
                    if(it->label == PC) break;
                    if(it->nxt != NULL) it = it->nxt;
                    else break;
                }
                 jump = 1;
            }
            else PC += 4;
        break;

        case 5: // BNE
            //printf("BNE\n");
            if(I_mem[input->rs] != I_mem[input->rt])
            {
                PC += 4 + input->imm*4;
                it = head;
                while(it != NULL){
                    if(it->label == PC) break;
                    if(it->nxt != NULL) it = it->nxt;
                    else break;
                }
                 jump =1;
            }
            else PC += 4;
        break;

        case 8:
            //printf("\n===\n imm = %d\n", input->imm);
            PC += 4;
            err_detect = ADDI(input);
        break;

        case 10:
            //printf("SLTI\n");
            PC += 4;
            err_detect = SLTI(input);
        break;

        case 12:
            //printf("ANDI\n");
            PC += 4;
            err_detect = ANDI(input);
        break;

        case 13:
            //printf("ORI\n");
            PC += 4;
            err_detect = ORI(input);
        break;


        case 32:
            //printf("LB\n");
            PC += 4;
            err_detect = LB(input);
        break;

        case 33:
            //printf("LHh\n");
            PC += 4;
            err_detect = LH(input);
        break;

        case 35:
            //printf("\n===\nimm = %d\n===\n===\n PC = %0x%08X\n", input->imm, PC+4);
            PC += 4;
            err_detect = LW(input);
        break;

        case 36:
            //printf("LBU\n");
            PC += 4;
            err_detect = LBU(input);
        break;

        case 37:
            //printf("LHU\n");
            PC += 4;
            err_detect = LHU(input);
        break;

        case 40:
            //printf("SB\n");
            PC += 4;
            err_detect = SB(input);
        break;

        case 41:
            //printf("SH\n");
            PC += 4;
            err_detect = SH(input);
        break;

        case 43:
            //printf("SW\n");
            PC += 4;
            err_detect = SW(input);
        break;

        case 63: //FF
            return;
        break;

        default:
        break;


     }
     print_mem(err_detect, cycle++);

     if(jump){
        traverse(it->nxt);
        jump = 0;
     }

     if(j_return){
        traverse(it->nxt);
        j_return = 0;
     }


}

void print_mem(int err_detect, int cycle)
{
  int j = 0;

  err_handle(err_detect, cycle);

  fprintf(output,"cycle %d\n", cycle);

  for(j = 0; j < 32; j++)
      fprintf(output, "$%02d: 0x%08X\n", j, I_mem[j]);
  fprintf(output, "PC: 0x%08X\n\n\n", PC);
  if(cycle %5 == 0 && cycle != 0);

}

void err_handle(int err_detect, int cycle)
{
    if(err_detect == 1234){ //four errors
        fprintf(error, "In cycle %d: Write $0 Error\n\n", cycle);
        fprintf(error, "In cycle %d: Number Overflow\n", cycle);
        fprintf(error, "In cycle %d: Address Overflow\n", cycle);
        fprintf(error, "In cycle %d: Misalignment Error\n", cycle);
        fclose(output);
        fclose(error);
        exit(1);
    }

    if(err_detect < 10){ // one error
        if (err_detect == 1) fprintf(error, "In cycle %d: Write $0 Error\n", cycle);
        if(err_detect == 2) fprintf(error, "In cycle %d: Number Overflow\n", cycle);
        if(err_detect == 3){
            fprintf(error, "In cycle %d: Address Overflow\n", cycle);
            fclose(output);
            fclose(error);
            exit(1);
        }
        if(err_detect == 4){
            fprintf(error, "In cycle %d: Misalignment Error\n", cycle);
            fclose(output);
            fclose(error);
            exit(1);
        }
    }
    else if(err_detect < 100){ //two errors
        int a = err_detect / 10;
        int b = err_detect % 10;
        switch (a) {
            case 1:
                fprintf(error, "In cycle %d: Write $0 Error\n", cycle);
                break;
            case 2:
                fprintf(error, "In cycle %d: Number Overflow\n", cycle);
                break;
            case 3:
                fprintf(error, "In cycle %d: Address Overflow\n", cycle);
                break;
        }
        switch (b) {
            case 2:
                fprintf(error, "In cycle %d: Number Overflow\n", cycle);
                break;
            case 3:
                fprintf(error, "In cycle %d: Address Overflow\n", cycle);
                fclose(output);
                fclose(error);
                exit(1);
                break;
            case 4:
                fprintf(error, "In cycle %d: Misalignment Error\n", cycle);
                fclose(output);
                fclose(error);
                exit(1);
                break;
        }

        if(a >= 3){
            fclose(output);
            fclose(error);
            exit(1);
        }
    }
    else if(err_detect < 1000){ // 3 errors
        int a = err_detect / 100;
        int b = (err_detect-a*100) / 10;
        int c = (err_detect- a*100 ) % 10;

        if (a == 1) fprintf(error, "In cycle %d: Write $0 Error\n", cycle);
        else if(a == 2) fprintf(error, "In cycle %d: Number Overflow\n", cycle);

        if(b == 2) fprintf(error, "In cycle %d: Number Overflow\n", cycle);
        else if(b == 3) fprintf(error, "In cycle %d: Address Overflow\n", cycle);

        if(c == 3) fprintf(error, "In cycle %d: Address Overflow\n", cycle);
        else if(c == 4) fprintf(error, "In cycle %d: Misalignment Error\n", cycle);

        fclose(output);
        fclose(error);
        exit(1);


    }

}

unsigned int swap(unsigned int x)
{
    x = (x>>24) |
    ((x<<8) & 0x00FF0000) |
    ((x>>8) & 0x0000FF00) |
    (x<<24);
    return x;
}


