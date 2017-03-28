#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "instruction.h"
#include "stage.h"
#include "token.h"
#include "print.h"

void do_func(instruct* input);
void traverse(instruct* input);
void read_iimage();
void read_dimage();
unsigned int swap(unsigned int);

FILE* iimage;
FILE* bimage;


instruct* head;

int main(int argc, char *argv[])
{

   iimage = fopen("iimage.bin", "rb");
   bimage = fopen("dimage.bin", "rb");
   
   initial();
   memset(I_mem, 0, sizeof(I_mem));
   memset(D_mem, 0, sizeof(D_mem));

   read_iimage();
   read_dimage();
   do_if(head);
   print_mem(0);
   print_pipe();
    PC += 4;
   traverse(head->nxt);

   fclose(output);

  return 0;
}

void traverse(instruct* input)
{
   int err = 0;
   int end_err = 0;
   int cycle = 1;
   int halt = 0;
    instruct *it;
   it = input;
    
   while(1)
   {
     print_mem(cycle++);
     //WB
     err = do_wb();
     print_err(err, cycle);
     //DM
     end_err = do_dm();
     print_err(end_err, cycle);
     //EX
     err = do_ex();
     print_err(err, cycle);
     //ID
     do_id();
     //IF
     do_if(it);
     print_pipe();
    
    if(it != NULL){
        if(it->op == 63 && WB != NULL && WB->op == 63)
            end_err = 1;
    }
      
     end_pipe(end_err);
      
     if(jump != 0){
         it = head;
         while(it != NULL){
             if(it->label == PC) break;
             if(it->nxt != NULL) it = it->nxt;
             else break;
         }

         if(it->nxt != NULL) it = it->nxt;
         else{
             while(PC < head->label-4){
                 print_mem(cycle++);
                 PC += 4;
                 //WB
                 err = do_wb();
                 print_err(err, cycle);
                 //DM
                 end_err = do_dm();
                 print_err(end_err, cycle);
                 //EX
                 err = do_ex();
                 print_err(err, cycle);
                 //ID
                 do_id();
                 //IF
                 do_if(NULL);
                 print_pipe();
             }
             it = head;
         }
     }
     else{
         if(stall == 0){
             PC += 4;
             if(it != NULL)
             it = it->nxt;
         }
     }
     
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

unsigned int swap(unsigned int x)
{
    x = (x>>24) |
    ((x<<8) & 0x00FF0000) |
    ((x>>8) & 0x0000FF00) |
    (x<<24);
    return x;
}


