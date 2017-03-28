#include <stdlib.h>
#include "instruction.h"

int no_add(int a, int b, int ans)
{
    if(a > 0 && b > 0 && ans <= 0)
        return  4;
    
    else if(a < 0 && b < 0 && ans >= 0)
        return  4;
    else return 0;
}

int no_sub(int a, int b, int ans)
{
    b *= (-1);
   
    if(a > 0 && b > 0 && ans <= 0)
        return  4;
    else if(a < 0 && b < 0 && ans >= 0)
        return  4;
    else return 0;
}

int no_I(int a, int b, int ans)
{
    if(a > 0 &&  b > 0 && ans < 0)
        return 4;
    else if(a < 0 &&  b < 0 && ans > 0)
        return 4;
    else return 0;
}