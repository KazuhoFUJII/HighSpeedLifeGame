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

    uint16_t r0,    r1,    r2;
    uint16_t r0_p1, r1_p1, r2_p1;
    uint16_t r0_p2, r1_p2, r2_p2;
    uint16_t nr;

    int i, j, k0, k1, k2, l;
    int s, w, o, e, n;
    int c, cp1, cp2;

    *na = 0;
    
    #pragma unroll NY/NUM_THREAD/2
    for (j =  tn      * (NY/NUM_THREAD); 
         j < (tn + 1) * (NY/NUM_THREAD);
         j++ ) {
        #pragma unroll
        for (i = 0; i < NC - 1; i++) {
            k0 = i + (j  ) * NC;
            k1 = i + (j+1) * NC;
            k2 = i + (j+2) * NC;
            /* read from currnet field */
            r0    = *(uint16_t *)(fld   +k0);
            r1    = *(uint16_t *)(fld   +k1);
            r2    = *(uint16_t *)(fld   +k2);

            r0_p1 = *(uint16_t *)(fld_p1+k0);
            r1_p1 = *(uint16_t *)(fld_p1+k1);
            r2_p1 = *(uint16_t *)(fld_p1+k2);

            r0_p2 = *(uint16_t *)(fld_p2+k0);
            r1_p2 = *(uint16_t *)(fld_p2+k1);
            r2_p2 = *(uint16_t *)(fld_p2+k2);

            nr = *(uint16_t *)(doa   +k1) & 0x1;

            #pragma unroll
            for (l = 0; l < 8; l++) {
                s = r0    & 0x2;
                w = r1    & 0x1;
                o = r1    & 0x2;
                e = r1    & 0x4;
                n = r2    & 0x2;

                c   = s | w << 1 | o << 2 | e << 3 | n << 4;

                s = r0_p1 & 0x2;
                w = r1_p1 & 0x1;
                o = r1_p1 & 0x2;
                e = r1_p1 & 0x4;
                n = r2_p1 & 0x2;

                cp1 = s | w << 1 | o << 2 | e << 3 | n << 4;

                s = r0_p2 & 0x2;
                w = r1_p2 & 0x1;
                o = r1_p2 & 0x2;
                e = r1_p2 & 0x4;
                n = r2_p2 & 0x2;

                cp2 = s | w << 1 | o << 2 | e << 3 | n << 4;

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

            *(uint16_t *)(doa + k1) = nr;
        }
    }
    return NULL;
}
