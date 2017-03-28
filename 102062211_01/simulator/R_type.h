#include "instruction.h"

int ADD(instruct* input)
{
  if(input->rd == 0) return 1;

  int ans = I_mem[input->rs] + I_mem[input->rt];

  if(I_mem[input->rs] > 0 && I_mem[input->rt] > 0 && ans < 0)
    return 2;

  else if(I_mem[input->rs] < 0 && I_mem[input->rt] < 0 && ans > 0)
    return 2;

  else
      I_mem[input->rd] = ans;

  return 0;
}

int SUB(instruct* input)
{
  if(input->rd == 0) return 1;

  int ans = I_mem[input->rs] - I_mem[input->rt];
  if(I_mem[input->rs] > 0 && I_mem[input->rt] < 0 && ans <= 0)
    return 2;
  else if(I_mem[input->rs] < 0 && I_mem[input->rs] > 0 && ans >= 0)
    return 2;
  else
  I_mem[input->rd] = ans;
  return 0;
}

int AND(instruct* input)
{
  if(input->rd == 0) return 1;

  I_mem[input->rd] = I_mem[input->rs] & I_mem[input->rt];

 return 0;
}

int OR(instruct* input)
{
  if(input->rd == 0) return 1;
  I_mem[input->rd] = I_mem[input->rs] | I_mem[input->rt];
  return 0;
}

int XOR(instruct* input)
{
  if(input->rd == 0) return 1;
  I_mem[input->rd] = I_mem[input->rs] ^ I_mem[input->rt];
  return 0;
}

int NOR(instruct* input)
{
  if(input->rd == 0) return 1;
  I_mem[input->rd] = ~(I_mem[input->rs] | I_mem[input->rt]);
  return 0;
}

int NAND(instruct* input)
{
  if(input->rd == 0) return 1;
  I_mem[input->rd] = ~(I_mem[input->rs] & I_mem[input->rt]);
  return 0;
}

int SLL(instruct* input)
{
  int reg = input->rd==0?1:0;

  if(input->rd && input->rs == 0 && input->rd == 0) reg = 0;
  else return reg;
  I_mem[input->rd] = I_mem[input->rs] << I_mem[input->rt];
  return 0;
}

int SRL(instruct* input)
{
  if(input->rd == 0) return 1;
  I_mem[input->rd] = I_mem[input->rs] >> I_mem[input->rt];
  return 0;
}

int SLT(instruct* input)
{
  if(input->rd == 0) return 1;
  if(I_mem[input->rs] < I_mem[input->rt])
    I_mem[input->rd] = 1;
  else I_mem[input->rd] = 0;
  return 0;
}

int SRA(instruct* input)
{
  if(input->rd == 0) return 1;
  int a = I_mem[input->rt];
  a /= 2^(input->shamt);
  I_mem[input->rd] = a;
  return 0;
}

