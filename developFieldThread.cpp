#include <pthread.h>
#include "etc.h"

uint_fast16_t blockCash[65536];

struct pth_arg{
    int tn;
    const uint8_t *fld;
    uint8_t *nfld;
};

#define NUM_BLOCK 4

void *developFieldThread(void *arg)
{
    struct pth_arg* parg = (pth_arg *)arg;
    int             tn = parg->tn;
    const uint8_t *fld = parg->fld;
    uint8_t      *nfld = parg->nfld;

    unsigned int r0, r1, r2, r3;
    unsigned int nr1, nr2;
    int i, j, k0, k1, k2, k3, l;

    uint_fast16_t blk, nblk;

    #pragma unroll NY/NUM_THREAD/2
    for(j =  tn      * (NY/NUM_THREAD); 
        j < (tn + 1) * (NY/NUM_THREAD);
        j += 2) {
        #pragma unroll
        for(i = 0; i < NC-1; i += NUM_BLOCK/4) {
            k0 = i + (j  ) * NC;
            k1 = i + (j+1) * NC;
            k2 = i + (j+2) * NC;
            k3 = i + (j+3) * NC;
            /* read from currnet field */
            r0 = *(unsigned int *)(fld+k0);
            r1 = *(unsigned int *)(fld+k1);
            r2 = *(unsigned int *)(fld+k2);
            r3 = *(unsigned int *)(fld+k3);            

            nr1 = nfld[k1] & 0x1;
            nr2 = nfld[k2] & 0x1;

            /* calc next block */
            #pragma unroll
            for(l = 0; l < 2*NUM_BLOCK; l+=2) {
                blk = (r0 & 0xF)       |
                      (r1 & 0xF) <<  4 |
                      (r2 & 0xF) <<  8 |
                      (r3 & 0xF) << 12;
                nblk = blockCash[blk];
                nr1 |= (nblk >> 4 & 0x6) << l;
                nr2 |= (nblk >> 8 & 0x6) << l;

                r0 >>= 2;
                r1 >>= 2;
                r2 >>= 2;
                r3 >>= 2;
            }
            
            /* write next blocks to next field */
            *(uint16_t *)(nfld+k1) = nr1;
            *(uint16_t *)(nfld+k2) = nr2;
        }
    }
    return NULL;
}



