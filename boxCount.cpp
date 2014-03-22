#include <stdlib.h>
#include <stdio.h>
#include "etc.h"

int *boxCount(const uint8_t* fld) {
    int *cnts = (int *)malloc(sizeof(int)*(LOGN+1));
    int w = 1;
    for (int lw = 0; lw < LOGN+1; lw++, w*=2) {
        int cnt = 0;
        for (int j = 0; j < NY; j += w) {
            for (int i = 0; i < ND; i += (w < 8 ? 1 : w/8)) {
                int l = i + j * NC;
                switch (w) {
                case 1: {
                    uint8_t c = fld[l];
                    if (c & 0x01) cnt++;
                    if (c & 0x02) cnt++;
                    if (c & 0x04) cnt++;
                    if (c & 0x08) cnt++;
                    if (c & 0x01) cnt++;
                    if (c & 0x02) cnt++;
                    if (c & 0x04) cnt++;
                    if (c & 0x08) cnt++;
                } break;
                case 2: {
                    uint8_t c0 = fld[l];
                    uint8_t c1 = fld[l+NC];
                    if (c0 & 0x03 || c1 & 0x03) cnt++; 
                    if (c0 & 0x0C || c1 & 0x0C) cnt++; 
                    if (c0 & 0x30 || c1 & 0x30) cnt++; 
                    if (c0 & 0xC0 || c1 & 0xC0) cnt++; 
                } break;
                case 4: {
                    uint8_t c0 = fld[l];
                    uint8_t c1 = fld[l+NC];
                    uint8_t c2 = fld[l+2*NC];
                    uint8_t c3 = fld[l+3*NC];
                    if (c0 & 0x0F || c1 & 0x0F || c2 & 0x0F || c3 & 0x0F) cnt++; 
                    if (c0 & 0xF0 || c1 & 0xF0 || c2 & 0xF0 || c3 & 0xF0) cnt++; 
                } break;
                default:
                    for (int p = 0; p < w/8; p++) {
                        for (int q = 0; q < w; q++) {
                            uint8_t c = fld[l+p+q*NC];
                            if (c) {
				cnt++;
				goto exist;
			    }
			}
                    }
exist: ;
                }
            }
        }
        cnts[lw] = cnt;
    }
    return cnts;
}

void printBoxCount(int *cnts_b, int *cnts_a, int nt) {
    if (!cnts_b && !cnts_a) return;
    char fname[50];
    sprintf(fname, "bxcnt/bxcnt%06d.txt", nt);
    FILE *file = fopen(fname, "w");
    if (!file) {
        fprintf(stderr, "cannot open file: %s\n", fname);
        return;
    }
    fprintf(file, "#boxsize	on	alive\n");
    int w = 1;
    for (int l = 0; l < LOGN+1; l++) {
#if DEAD_OR_ALIVE > 0    
        fprintf(file, "%d	%d	%d\n", w, cnts_b[l], cnts_a[l]);
#else
        fprintf(file, "%d	%d	%d\n", w, cnts_b[l], 0);
#endif
	w *= 2;
    }
    fclose(file);
}
