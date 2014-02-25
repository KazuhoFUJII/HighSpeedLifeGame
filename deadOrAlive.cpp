#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include "etc.h"

void *deadOrAliveThread(void *arg);

struct pth_arg {
    int tn;
    const uint8_t *fld;
    const uint8_t *fld_p1;
    const uint8_t *fld_p2;
    uint8_t *doa;
    int *na;
};

int deadOrAlive(
    const uint8_t *fld,
    const uint8_t *fld_p1,
    const uint8_t *fld_p2,
    uint8_t *doa
    )
{
    static pthread_t pt[NUM_THREAD];
    struct pth_arg arg[NUM_THREAD];
    int na[NUM_THREAD];

    #pragma unroll
    for (int i = 0; i < NUM_THREAD; i++) {
        arg[i].tn     = i;
        arg[i].fld    = fld;
        arg[i].fld_p1 = fld_p1;
        arg[i].fld_p2 = fld_p2;
        arg[i].doa    = doa;
        arg[i].na     = na + i;
        pthread_create(&pt[i], NULL, &deadOrAliveThread, (void *)&arg[i]);
    }
    #pragma unroll
    for (int i = 0; i < NUM_THREAD; i++) {
        pthread_join(pt[i], NULL);
    }
    #pragma unroll
    int ret = 0;
    for (int i = 0; i < NUM_THREAD; i++) {
        ret += na[i];
    }
    return ret;
}

void *deadOrAliveThread(void *arg)
{
    struct pth_arg* parg = (pth_arg *)arg;
    int            tn     = parg->tn;
    const uint8_t *fld    = parg->fld;
    const uint8_t *fld_p1 = parg->fld_p1;
    const uint8_t *fld_p2 = parg->fld_p2;
    uint8_t       *doa    = parg->doa;
    int           *na     = parg->na;

    *na = 0;
    
    #pragma unroll NY/NUM_THREAD/2
    for (int j = tn*(NY/NUM_THREAD); 
         j < (tn+1)*(NY/NUM_THREAD);
         j++ ) {
        #pragma unroll
        for (int i = 0; i < ND; i++) {
            int k0 = i + (j  )*NC;
            int k1 = i + (j+1)*NC;
            int k2 = i + (j+2)*NC;
            /* read from currnet field */
            scan_t r0    = *(scan_t *)(fld   +k0);
            scan_t r1    = *(scan_t *)(fld   +k1);
            scan_t r2    = *(scan_t *)(fld   +k2);

            scan_t r0_p1 = *(scan_t *)(fld_p1+k0);
            scan_t r1_p1 = *(scan_t *)(fld_p1+k1);
            scan_t r2_p1 = *(scan_t *)(fld_p1+k2);

            scan_t r0_p2 = *(scan_t *)(fld_p2+k0);
            scan_t r1_p2 = *(scan_t *)(fld_p2+k1);
            scan_t r2_p2 = *(scan_t *)(fld_p2+k2);

            scan_t nr = *(scan_t *)(doa   +k1) & 0x1;

            #pragma unroll
            for (int l = 0; l < sizeof(scan_t)*8/2; l++) {
                int b, m, t;
                b  = r0    & 0x7;
                m  = r1    & 0x7;
                t  = r2    & 0x7;
                int c   = b | m << 3 | t << 6;

                b  = r0_p1 & 0x7;
                m  = r1_p1 & 0x7;
                t  = r2_p1 & 0x7;
                int cp1 = b | m << 3 | t << 6;

                b  = r0_p2 & 0x7;
                m  = r1_p2 & 0x7;
                t  = r2_p2 & 0x7;
                int cp2 = b | m << 3 | t << 6;

                if (c == 0x0 || c == cp1 || c == cp2) {
                    nr |= 0;
                } else {
                    nr |=  0x2 << l;
                    (*na)++;
                }
                r0    >>= 1;
                r1    >>= 1;
                r2    >>= 1;

                r0_p1 >>= 1;
                r1_p1 >>= 1;
                r2_p1 >>= 1;
                
                r0_p2 >>= 1;
                r1_p2 >>= 1;
                r2_p2 >>= 1;
            }
            *(scan_t *)(doa + k1) = nr;
        }
    }
    return NULL;
}
