#ifndef _instruction_h
#define _instruction_h

#include<stdlib.h>
int I_mem[32];
int D_mem[1024];
int PC;
int SP;
int b_count;

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
int code;
};
typedef struct instruct instruct;

#endif



