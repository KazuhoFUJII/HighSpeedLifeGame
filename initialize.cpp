#include <stdio.h>
#include <stdlib.h>
#include <random>
#include "etc.h"

void cellCashInitialize();
void blockCashInitialize();

void initialize(uint8_t *field)
{
    std::mt19937 mt;
    double seed = SEED*mt.max();

    for(int j = 0; j < NY; j++)
        for(int i = 0; i < NC-1; i++){
            uint8_t cell = 0x00;
            for(int d = 0; d < 8*sizeof(uint8_t); d++){
                cell = cell << 1 | (mt() < seed ? 0x01 : 0x00);
            }
            field[i+j*NC] = cell;
        }
    // glider
    //field[   0] = 0x04;
    //field[  NC] = 0x03;
    //field[2*NC] = 0x06;
    
    cellCashInitialize();
    blockCashInitialize();
}

extern uint_fast16_t blockCash[65536]; 
extern uint_fast16_t cellCash[512]; //2^9

uint_fast16_t developCellCash(uint_fast16_t block);
uint_fast16_t developCellCalc(uint_fast16_t block);

static inline uint_fast16_t developBlockCalc(uint_fast16_t block)
{
    block =        
        developCellCash(block     )      |
        developCellCash(block >> 1) << 1 |
        developCellCash(block >> 4) << 4 |
        developCellCash(block >> 5) << 5;
    return block;
}

void blockCashInitialize()
{
    for(int b = 0; b < 65536; b++){ 
        blockCash[b] = developBlockCalc((uint_fast16_t)b);
    }
}

void cellCashInitialize(){
    for(int i = 0; i < 512; i++){
        uint_fast16_t n = 
            (i & 0x1C0) << 2 |
            (i & 0x038) << 1 |
            (i & 0x007);
        cellCash[i] = developCellCalc(n);
    }
}

