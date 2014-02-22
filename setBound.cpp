#include <string.h>
#include "etc.h"

void setBound1(uint8_t *field)
{
    #pragma unroll
    for(int j = 0; j < NY; j++){
        field[NC-1+j*NC] = field[j*NC];
    }
    memcpy(field+NY*NC, field, 2*NC);
}

void setBound2(uint8_t *field)
{
    #pragma unroll
    for(int j = 0; j < NY+1; j++){
        field[j*NC] = field[     j*NC] & ~(uint8_t)0x1 | 
                      field[NC-1+j*NC] &  (uint8_t)0x1;
    }
    memcpy(field, field+NY*NC, NC);
}
