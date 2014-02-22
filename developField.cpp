#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include "etc.h"

void *developFieldThread(void *);

struct pth_arg{
    int tn;
    const uint8_t *fld;
    uint8_t *nfld;
};

void developField(const uint8_t *fld, uint8_t *nfld){
    static pthread_t pt[NUM_THREAD];
    struct pth_arg arg[NUM_THREAD];

    #pragma unroll
    for(int i = 0; i < NUM_THREAD; i++){
        arg[i].tn   = i;
        arg[i].fld  = fld;
        arg[i].nfld = nfld;
        pthread_create(&pt[i], NULL, &developFieldThread, (void *)&arg[i]);
    }
    #pragma unroll
    for(int i = 0; i < NUM_THREAD; i++){
        pthread_join(pt[i], NULL);
    }
}

