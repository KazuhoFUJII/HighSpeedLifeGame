#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include "etc.h"

void initialize(uint8_t *field);
void* printPBM(void *);
void setBound1(uint8_t *field);
void setBound2(uint8_t *field);
void developField(const uint8_t *field, uint8_t *nfield);

struct st_field{
    uint8_t *field;
    int nt;
};

int main(int argc, char *argv[])
{
    pthread_t pbm;

    /* initialize */
    uint8_t *field  = (uint8_t*)calloc(NC*(NY+2), sizeof(uint8_t));
    uint8_t *nfield = (uint8_t*)calloc(NC*(NY+2), sizeof(uint8_t));

    time_t start = clock();
    initialize(field);

#if PRINT_PBM == 1
    struct st_field sf = {field, 0};
    pthread_create(&pbm, NULL, printPBM, (void *)&sf);
#endif

    /* main loop */
    #pragma unroll
    for(int nt = 1; nt <= EXEC_STEP; nt++){
        if(!(nt % 100)){
            printf("nt= %08d\n", nt);
        }
        setBound1(field);
        developField(field, nfield);
        
        //swap
        uint8_t *tmp = nfield;
        nfield = field;
        field = tmp;
        
        setBound2(field);

#if PRINT_PBM == 1
        pthread_join(pbm, NULL);
        sf.field = field;
        sf.nt = nt;
        pthread_create(&pbm, NULL, printPBM, (void *)&sf);
#endif
    }

    printf("TIME: %ld [ms]\n", (clock() - start)*1000/CLOCKS_PER_SEC);

#if PRINT_PBM == 1
    pthread_join(pbm, NULL);
#endif

    free(field);
    free(nfield);
    return EXIT_SUCCESS;
}
