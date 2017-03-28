#include<stdlib.h>

int I_mem[32];
int D_mem[1024];
int PC;
int SP;
int b_count;
#ifndef _instruct_
#define _instruct_

struct instruct{

int op;
int rs;
int rt;
int rd;
int shamt;
int func;
unsigned int address;
int imm;
int label;
struct instruct* nxt;
struct instruct* pre;
};
typedef struct instruct instruct;

#endif



