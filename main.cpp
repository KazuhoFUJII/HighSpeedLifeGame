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
    const char* str;
};

int main(int argc, char *argv[])
{
    /* initialize */
    uint8_t *field_n;  // field in next step
    uint8_t *field;    // field in this step
    uint8_t *field_p1; // field in previous step
    uint8_t *field_p2; // field in previous previous step

    field_n  = (uint8_t *)calloc(NC*(NY+2), sizeof(uint8_t));
    field    = (uint8_t *)calloc(NC*(NY+2), sizeof(uint8_t));
    field_p1 = (uint8_t *)calloc(NC*(NY+2), sizeof(uint8_t));
    field_p2 = (uint8_t *)calloc(NC*(NY+2), sizeof(uint8_t));

    struct st_field sf;
#if DEAD_OR_ALIVE == 1
    pthread_t th_doa;
    uint8_t *doa; // dead or alive
    doa = (uint8_t *)calloc(NC*(NY+2), sizeof(uint8_t));
#if PRINT_PBM == 2
    sf.field = doa;
    sf.nt = 0;
    sf.str = "da";
    pthread_create(&th_doa, NULL, printPBM, (void *)&sf);
#endif
#endif
    time_t start = clock();
    initialize(field);

#if PRINT_PBM >= 1
    sf.field = field;
    sf.nt = 0;
    sf.str = "bw";
    pthread_t th_pbm;
    pthread_create(&th_pbm, NULL, printPBM, (void *)&sf);
#endif

    /* main loop */
    #pragma unroll
    for (int nt = 1; nt <= EXEC_STEP; nt++) {
        if (!(nt % 100)) {
            fprintf(stderr, "nt= %06d\n", nt);
        }
        setBound1(field);
        int black = developField(field, field_n);
        //swap
        uint8_t *tmp = field_p2;
        field_p2 = field_p1;
        field_p1 = field;
        field    = field_n;
        field_n  = tmp;

        setBound2(field);

#if PRINT_PBM >= 1
        sf.field = field;
        sf.nt = nt;
        sf.str = "bw";
        pthread_join(th_pbm, NULL);
        pthread_create(&th_pbm, NULL, printPBM, (void *)&sf);
#endif
        
#if DEAD_OR_ALIVE == 1
        int alive = deadOrAlive(field, field_p1, field_p2, doa);
        setBound2(doa);
        fprintf(stdout, "%d	%d	%d", nt, black, alive);
#if PRINT_PBM == 2
        sf.field = doa;
        sf.nt = nt;
        sf.str = "da";
	pthread_join(th_doa, NULL);
        pthread_create(&th_doa, NULL, printPBM, (void *)&sf);
#endif
#endif
        fprintf(stdout, "\n");
    }

    fprintf(stderr, "TIME: %ld [ms]\n", (clock() - start)*1000/CLOCKS_PER_SEC);

#if PRINT_PBM >= 1
    pthread_join(th_pbm, NULL);
#endif
#if DEAD_OR_ALIVE == 1 && PRINT_PBM == 2
    pthread_join(th_doa, NULL);
#endif
    free(field_n);
    free(field);
    free(field_p1);
    free(field_p2);

    return EXIT_SUCCESS;
}
