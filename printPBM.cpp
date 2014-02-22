#include <stdio.h>
#include <string.h>
#include <math.h>
#include "etc.h"

#define PBM_HEADER 25

struct st_field{
    uint8_t *field;
    int nt;
};

void* printPBM(void* sf)
{
    uint8_t *field = ((st_field *)sf)->field;
    int nt = ((st_field *)sf)->nt;

    uint8_t tmp[NX/CHAR_BIT/sizeof(uint8_t)*NY + PBM_HEADER];
    sprintf((char *)tmp, "P4\n#%08d\n%5d %5d\n", nt, NX, NY);

    // reverse bit order
    #pragma unroll
    for(int j = 0; j < NY; j++)
        #pragma unroll
        for(int i = 0; i < NX/CHAR_BIT/sizeof(uint8_t); i++){
            uint8_t c = field[i+j*NC];
            tmp[i + j*NX/CHAR_BIT + PBM_HEADER] = 
                c << 7        |
                c << 5 & 0x40 |
                c << 3 & 0x20 |
                c << 1 & 0x10 |
                c >> 1 & 0x08 |
                c >> 3 & 0x04 | 
                c >> 5 & 0x02 |
                c >> 7;
        }

    //write file
    char fname[99];    
    sprintf(fname, "./pbm/%08d.pbm", nt);
    FILE *fp = fopen(fname, "wb");
    if(fp == NULL){
        printf("\n\nERR: CAN NOT OPEN %s\n", fname);
        return NULL;
    }
    
    fwrite(tmp, sizeof(uint8_t), 
           NX*NY/CHAR_BIT/sizeof(uint8_t) + PBM_HEADER, fp);

    fclose(fp);

    return NULL;
}

