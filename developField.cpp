#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include "etc.h"

void *developFieldThread(void *);

struct pth_arg {
    int tn;
    const uint8_t *fld;
    uint8_t *nfld;
    int *nb;
};

uint_fast16_t blockCash[65536];

int developField(const uint8_t *fld, uint8_t *nfld)
{
    static pthread_t pt[NUM_THREAD];
    struct pth_arg arg[NUM_THREAD];
    int nb[NUM_THREAD];

    #pragma unroll
    for (int i = 0; i < NUM_THREAD; i++) {
        arg[i].tn   = i;
        arg[i].fld  = fld;
        arg[i].nfld = nfld;
        nb[i] = 0;
	arg[i].nb = nb + i;
        pthread_create(&pt[i], NULL, 
	               &developFieldThread, (void *)&arg[i]);
    }
    #pragma unroll
    for (int i = 0; i < NUM_THREAD; i++) {
        pthread_join(pt[i], NULL);
    }
    int ret = 0;
    #pragma unroll
    for (int i = 0; i < NUM_THREAD; i++) {
        ret += nb[i];
    }
    return ret;
}

void *developFieldThread(void *arg)
{
    struct pth_arg* parg = (pth_arg *)arg;
    int             tn = parg->tn;
    const uint8_t *fld = parg->fld;
    uint8_t      *nfld = parg->nfld;
    int            *nb = parg->nb;
 
    #pragma unroll NY/NUM_THREAD/2
    for (int j = tn*(NY/NUM_THREAD); 
         j < (tn+1)*(NY/NUM_THREAD);
         j += 2) {
        #pragma unroll
        for (int i = 0; 
             i < ND; 
             i += SCAN_BLOCK*2/sizeof(uint8_t)/8) {
            int k0 = i + (j  )*NC;
            int k1 = i + (j+1)*NC;
            int k2 = i + (j+2)*NC;
            int k3 = i + (j+3)*NC;
            /* read from currnet field */
            scan_t r0 = *(scan_t *)(fld+k0);
            scan_t r1 = *(scan_t *)(fld+k1);
            scan_t r2 = *(scan_t *)(fld+k2);
            scan_t r3 = *(scan_t *)(fld+k3);

            scan_t nr1 = nfld[k1] & 0x1;
            scan_t nr2 = nfld[k2] & 0x1;

            /* calc next block */
            #pragma unroll
            for (int l = 0;
                 l < 2*SCAN_BLOCK; 
                 l += 2) {
                uint_fast16_t blk = 
                      (r0 & 0xF)       |
                      (r1 & 0xF) <<  4 |
                      (r2 & 0xF) <<  8 |
                      (r3 & 0xF) << 12;
                uint_fast16_t nblk = blockCash[blk];
                nr1 |= (scan_t)(nblk >> 4 & 0x6) << l;
                nr2 |= (scan_t)(nblk >> 8 & 0x6) << l;

                r0 >>= 2;
                r1 >>= 2;
                r2 >>= 2;
                r3 >>= 2;

                /* sum of true cell */
                switch (nblk) {
                    case 0x0000:
                        break;
                    case 0x0020:
                    case 0x0040:
                    case 0x0200:
                    case 0x0400:
                        (*nb)++;
                        break;
                    case 0x0060:
                    case 0x0600:
                    case 0x0220:
                    case 0x0440:
                    case 0x0240:
                    case 0x0420:
                        *nb += 2;
                        break;
                    case 0x0640:
                    case 0x0620:
                    case 0x0460:
                    case 0x0260:
                        *nb += 3;
                        break;
                    case 0x0660:
                        *nb += 4;
                        break;
                }
            }
            
            /* write next blocks to next field */
            *(scan_t *)(nfld+k1) = nr1;
            *(scan_t *)(nfld+k2) = nr2;
        }
    }
    return NULL;
}
