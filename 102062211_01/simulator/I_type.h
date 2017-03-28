#include "instruction.h"
#include <stdio.h>

int ADDI(instruct* input)
{
    int err = 0;
    if(input->rt == 0)  err = 1;

    int ans = I_mem[input->rs] + input->imm;
    if(I_mem[input->rs] > 0 && input->imm > 0 && ans <= 0){
        if(err == 1) err =  12;
        else err = 2;

    }
    else if(I_mem[input->rs] < 0 && input->imm < 0 && ans >= 0){
        if(err == 1) err =  12;
        else err = 2;

    }
     if(input->rt != 0)
        I_mem[input->rt] = I_mem[input->rs] + input->imm;

    return err;
}

int SLTI(instruct* input)
{
    if(input->rt== 0) return 1;

    if(I_mem[input->rs] < input->imm)
        I_mem[input->rt] = 1;
    else
        I_mem[input->rt] = 0;
    return 0;
}

int ANDI(instruct* input)
{
    if(input->rt == 0) return 1;
    unsigned int a = (unsigned int) input->imm;
    I_mem[input->rt] = I_mem[input->rs] & a;
    return 0;
}

int ORI(instruct* input)
{
    if(input->rt == 0) return 1;
    unsigned int a = (unsigned int) input->imm;
    I_mem[input->rt] = I_mem[input->rs] | a;
    return 0;
}

int NORI(instruct* input)
{
    if(input->rt == 0) return 1;
    unsigned int a = (unsigned int) input->imm;
    I_mem[input->rt] = ~(I_mem[input->rs] | a);
    return 0;
}

int LUI(instruct* input)
{
    if(input->rt == 0) return 1;
    I_mem[input->rt] = input->imm << 16;
    return 0;
}

int LB(instruct* input) // 1 byte from memory  signed
{
    int err = 0;
    if(input->rt == 0) err = 1;
    if(I_mem[input->rs] > 0 &&  input->imm > 0 && I_mem[input->rs] + input->imm < 0)
        err = err*10+2;
    else if(I_mem[input->rs] < 0 &&  input->imm < 0 && I_mem[input->rs] + input->imm > 0)
        err = err*10+2;
    if(I_mem[input->rs] + input->imm > 1023 || I_mem[input->rs] + input->imm < 0){
        err = err*10+3;
        return err;
    }

    int b = D_mem[I_mem[input->rs] + input->imm/4] << 8*(input->imm%4);
    int c = b >> 8*3;

    I_mem[input->rt] = c;
    return err;
}

int LBU(instruct* input) // 1 byte unsigned
{
    int err = 0;
    if(input->rt == 0) err = 1;
    if(I_mem[input->rs] > 0 &&  input->imm > 0 && I_mem[input->rs] + input->imm < 0)
        err = err*10+2;
    else if(I_mem[input->rs] < 0 &&  input->imm < 0 && I_mem[input->rs] + input->imm > 0)
        err = err*10+2;

    if(I_mem[input->rs] + input->imm > 1023 || I_mem[input->rs] + input->imm < 0){
        err = err*10+3;
        return err;
    }


    if(I_mem[input->rs] + input->imm <= 1023 && I_mem[input->rs] + input->imm >= 0 && input->rt != 0){

        unsigned int b = D_mem[I_mem[input->rs] + input->imm/4] << 8*(input->imm%4);
        unsigned int c = b >> 8*3;


        I_mem[input->rt] = c;

    }

    return err;
}

int SH(instruct* input) // 2 bytes to memory
{

    int err = 0;
    if(I_mem[input->rs] > 0 &&  input->imm > 0 && I_mem[input->rs] + input->imm < 0)
        err = err*10+2;
    else if(I_mem[input->rs] < 0 &&  input->imm < 0 && I_mem[input->rs] + input->imm > 0)
        err = err*10+2;
    if(I_mem[input->rs] + input->imm > 1023 || I_mem[input->rs] + input->imm < 0)
        err = err*10+3;
    if(input->imm % 2 != 0) err = err*10 + 4;


    if(I_mem[input->rs] + input->imm <= 1023 && I_mem[input->rs] + input->imm >= 0 && input->imm % 2 == 0){

        int a  = I_mem[input->rt] & 0x0000FFFF;


        if(input->imm %4 == 0){
            int c = D_mem[I_mem[input->rs] + input->imm/4] & 0x0000FFFF;
            D_mem[I_mem[input->rs] + input->imm/4] = (a << 16) + c;

        }
        else if(input->imm %4 == 2){
            int c = D_mem[I_mem[input->rs] + input->imm/4] & 0xFFFF0000;
            D_mem[I_mem[input->rs] + input->imm/4] = a + c;
        }


    }

    return err;
}

int SB(instruct* input) // 1 byte to memory
{
    int err = 0;
    if(I_mem[input->rs] > 0 &&  input->imm > 0 && I_mem[input->rs] + input->imm < 0)
        err = err*10+2;
    else if(I_mem[input->rs] < 0 &&  input->imm < 0 && I_mem[input->rs] + input->imm > 0)
        err = err*10+2;
    if(I_mem[input->rs] + input->imm > 1023 || I_mem[input->rs] + input->imm < 0)
        err = err*10+3;
    if(input->imm % 2 != 0) err = err*10 + 4;

    if(I_mem[input->rs] + input->imm <= 1023 && I_mem[input->rs] + input->imm >= 0){

        int a = input->rt & 0x0000FFFF;

        if(input->imm % 4 == 0){
            int b = D_mem[I_mem[input->rs] + input->imm/4] & 0x00FFFFFFFF;
            D_mem[I_mem[input->rs] + input->imm/4] = (a << 24) + b;

        }
        else if(input->imm % 4 == 1){
            int b = D_mem[I_mem[input->rs] + input->imm/4] & 0xFF00FFFF;
            D_mem[I_mem[input->rs] + input->imm/4] = (a << 16) + b;

        }
        else if(input->imm % 4 == 2){
            int b = D_mem[I_mem[input->rs] + input->imm/4] & 0xFFFF00FF;
            D_mem[I_mem[input->rs] + input->imm/4] = (a << 8) + b;

        }
        else if(input->imm % 4 == 3){
            int b = D_mem[I_mem[input->rs] + input->imm/4] & 0xFFFFFF00;
            D_mem[I_mem[input->rs] + input->imm/4] = a  + b;

        }

    }

    return err;
}

int LH(instruct* input) // 2 bytes from  memory signed
{
    int err = 0;
    if(input->rt == 0) err =  1;
    if(I_mem[input->rs] > 0 &&  input->imm > 0 && I_mem[input->rs] + input->imm < 0)
        err = err*10+2;
    else if(I_mem[input->rs] < 0 &&  input->imm < 0 && I_mem[input->rs] + input->imm > 0)
        err = err*10+2;
    if(I_mem[input->rs] + input->imm > 1023 || I_mem[input->rs] + input->imm < 0)
        err = err*10+3;
    if(input->imm % 2 != 0) err =   err*10 + 4;

    if(I_mem[input->rs] + input->imm <= 1023 && I_mem[input->rs] + input->imm >= 0 && input->imm % 2 == 0){

        if(input->rt != 0){
            if(input->imm % 4 == 0)
                I_mem[input->rt] = (D_mem[input->rs + input->imm/4] & 0xFFFF0000) >> 16;
            else if(input->imm % 4 == 2)
                I_mem[input->rt] = D_mem[input->rs + input->imm/4] & 0x0000FFFF;
            if(I_mem[input->rt] >> 15 == 1) I_mem[input->rt] = I_mem[input->rt] | 0xFFFF0000;
        }
    }

        return err;
}

int LHU(instruct* input) //2 bytes from memory unsigned
{
    int err = 0;
    if(input->rt == 0) err = 1;
    if(I_mem[input->rs] > 0 &&  input->imm > 0 && I_mem[input->rs] + input->imm < 0)
        err = err*10+2;
    else if(I_mem[input->rs] < 0 &&  input->imm < 0 && I_mem[input->rs] + input->imm > 0)
        err = err*10+2;
    if(I_mem[input->rs] + input->imm > 1023 || I_mem[input->rs] + input->imm < 0)
        err = err*10+3;
    if(input->imm % 2 != 0) err =   err*10 + 4;

    if(I_mem[input->rs] + input->imm <= 1023 && I_mem[input->rs] + input->imm >= 0 && input->imm % 2 == 0){

        if(input->rt != 0){
            if(input->imm % 4 == 0)
                I_mem[input->rt] = (D_mem[I_mem[input->rs] + input->imm/4] & 0xFFFF0000) >> 16;
            else if(input->imm % 4 == 2)
                I_mem[input->rt] = D_mem[I_mem[input->rs] + input->imm/4] & 0x0000FFFF;
        }

    }

    return err;
}

int LW(instruct* input) //load 4 bytes
{
    int err = 0;
    if(input->rt == 0 ) err =  1;
    if(I_mem[input->rs] > 0 &&  input->imm > 0 && I_mem[input->rs] + input->imm < 0)
        err = err*10+2;
    else if(I_mem[input->rs] < 0 &&  input->imm < 0 && I_mem[input->rs] + input->imm > 0)
        err = err*10+2;
    if(I_mem[input->rs] + input->imm > 1023 || I_mem[input->rs] + input->imm < 0)
        err = err*10+3;
    if(input->imm % 4 != 0) err =  err*10 + 4;

    if(I_mem[input->rs] + input->imm <= 1023 && I_mem[input->rs] + input->imm >= 0 && input->imm % 4 == 0){

        if(input->rt != 0)
            I_mem[input->rt] = D_mem[I_mem[input->rs] + input->imm/4];

    }

    return err;

}

int SW(instruct* input) //save 4 bytes
{
    int err = 0;
    if(input->rt == 0 ) err =  1;
    if(I_mem[input->rs] > 0 &&  input->imm > 0 && I_mem[input->rs] + input->imm < 0)
        err = err*10+2;
    else if(I_mem[input->rs] < 0 &&  input->imm < 0 && I_mem[input->rs] + input->imm > 0)
        err = err*10+2;
    if(I_mem[input->rs] + input->imm > 1023 || I_mem[input->rs] + input->imm < 0)
        err = err*10+3;
    if(input->imm % 4 != 0) err =  err*10 + 4;

    if(I_mem[input->rs] + input->imm <= 1023 && I_mem[input->rs] + input->imm >= 0 && input->imm % 4 == 0)
        D_mem[I_mem[input->rs] + input->imm/4] = I_mem[input->rt];

    return err;
}
