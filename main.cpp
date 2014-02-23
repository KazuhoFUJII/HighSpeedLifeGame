#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include "etc.h"

void initialize(uint8_t *field);
void* printPBM(void *);
void setBound1(uint8_t *field);
void setBound2(uint8_t *field);
int developField(const uint8_t *field, uint8_t *nfield);
int deadOrAlive(const uint8_t* fld, const uint8_t* fld_p1, 
                const uint8_t* fld_p2, uint8_t* doa);

struct st_field{
    uint8_t *field;
    int nt;
};

int main(int argc, char *argv[])
{
    /* initialize */
    uint8_t *field_n;  // field in next step
    uint8_t *field;    // field in this step
    uint8_t *field_p1; // field in previous step
    uint8_t *field_p2; // field in previous previous step

    field_n  = (uint8_t*)calloc(NC*(NY+2), sizeof(uint8_t));
    field    = (uint8_t*)calloc(NC*(NY+2), sizeof(uint8_t));
    field_p1 = (uint8_t*)calloc(NC*(NY+2), sizeof(uint8_t));
    field_p2 = (uint8_t*)calloc(NC*(NY+2), sizeof(uint8_t));

#if DEAD_OR_ALIVE == 1
    pthread_t th_doa;
    uint8_t *doa; // dead or alive
    doa = (uint8_t*)calloc(NC*(NY+2), sizeof(uint8_t));
#endif
    time_t start = clock();
    initialize(field);

#if PRINT_PBM == 1
    struct st_field sf = {field, 0};
    pthread_t th_pbm;
    pthread_create(&th_pbm, NULL, printPBM, (void *)&sf);
#endif

    /* main loop */
    #pragma unroll
    for (int nt = 1; nt <= EXEC_STEP; nt++) {
        if (!(nt % 100)) {
            fprintf(stderr, "nt= %08d\n", nt);
        }
        setBound1(field);
        int black = developField(field, field_n);
	fprintf(stdout, "%d	%d", nt, black);
        //swap
        uint8_t *tmp = field_p2;
        field_p2 = field_p1;
        field_p1 = field;
        field    = field_n;
        field_n  = tmp;

        setBound2(field);

#if PRINT_PBM == 1
        sf.field = field;
        sf.nt = nt;
        pthread_join(th_pbm, NULL);
        pthread_create(&th_pbm, NULL, printPBM, (void *)&sf);
#endif
        
#if DEAD_OR_ALIVE == 1
        int alive = deadOrAlive(field, field_p1, field_p2, doa);
        setBound2(doa);
        fprintf(stdout, "	%d", alive);
#endif
        fprintf(stdout, "\n");
    }

    fprintf(stderr, "TIME: %ld [ms]\n", (clock() - start)*1000/CLOCKS_PER_SEC);

#if PRINT_PBM == 1
    pthread_join(th_pbm, NULL);
#endif

    free(field_n);
    free(field);
    free(field_p1);
    free(field_p2);

    return EXIT_SUCCESS;
}
