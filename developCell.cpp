#include <stdio.h>
#include "etc.h"

uint_fast16_t cellCash[512]; //2^9

uint_fast16_t developCellCalc(uint_fast16_t block)
{
    // 3: 0 0 0 0
    // 2: 0 N N N
    // 1: 0 N C N
    // 0: 0 N N N
    int sum  = // sum of alive cells in neighbor
        (block       & 0x0001) + 
        (block >>  1 & 0x0001) + 
        (block >>  2 & 0x0001) +
        (block >>  4 & 0x0001) +
        (block >>  6 & 0x0001) +
        (block >>  8 & 0x0001) +
        (block >>  9 & 0x0001) +
        (block >> 10 & 0x0001);

    if(sum == 3 || sum == 2 && block & 0x0020){
        return 0x20;
    }
    return 0x00;
}

uint_fast16_t developCellCash(uint_fast16_t block)
{
    int n = 
        (block & 0x0700) >> 2 | 
        (block & 0x0070) >> 1 | 
        (block & 0x0007);
    return cellCash[n];
}



