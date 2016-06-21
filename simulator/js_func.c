#include <stdio.h>
#include "get_images.h"
//return halt?
int js_func(int cycle, FILE* err_file, int *PC, int *reg, int *mem, int type_num, int C){

    switch(type_num){
        unsigned int tmp1;
        int tmp2;
        *PC += 4;

        case J:
            //printf("J:  C = %08x\n", C);
            tmp1 = C<<6;
            tmp1 = tmp1>>4;
            tmp2 = (*PC)&0xf0000000;
            *PC = tmp1+tmp2;
            break;
        case JAL:
            //printf("JAL:  C = %08x\n", C);
            reg[31] = *PC + 4;
            tmp1 = C<<6;
            tmp1 = tmp1>>4;
            tmp2 = (*PC)&0xf0000000;
            *PC = tmp1+tmp2;
            //printf("PC = %08x\n", *PC);
            break;
        case HALT:
            break;
        default:
            break;
    }
    return 0;
}
