#include <stdio.h>
#include "get_images.h"
//return halt?
int r_func(int cycle, FILE* err_file, int *PC, int *reg, int *mem, int type_num, unsigned int rs, unsigned int rt, unsigned int rd, int C){
    //int return_num = -1; // 0 -> continue;  1->halt
    int return_num2 = -1;
    int tmp;
    int i,j;
    *PC += 4;
    //write $0 error
    if(type_num != JR){
        if(rd == 0){
            if(type_num != SLL || C != 0 || rt != 0){
                fprintf(err_file, "In cycle %d: Write $0 Error\n", cycle);
                printf("In cycle %d: Write $0 Error\n", cycle);
                return_num2 = 0;
            }
        }
    }
    //num overflow error
    if(type_num == ADD){
        printf("ADD  %02x == %02x\n", type_num, ADD);
        tmp = reg[rs] + reg[rt];
        //printf("tmp = %08x, reg[rs] = %08x, reg[rt] = %08x\n", tmp, reg[rs], reg[rt]);
        if(reg[rs]>=0 && reg[rt]>=0 && tmp < 0){
            //printf("In cycle %d: Number Overflow\n", cycle);
            fprintf(err_file, "In cycle %d: Number Overflow\n", cycle);
            printf("In cycle %d: Number Overflow\n", cycle);
            //return_num = -1;
        }else if(reg[rs]<0 && reg[rt]<0 && tmp >=0){
            //printf("In cycle %d: Number Overflow\n", cycle);
            fprintf(err_file, "In cycle %d: Number Overflow\n", cycle);
            printf("In cycle %d: Number Overflow\n", cycle);
            //return_num = -1; // -1 keep going on
        }
    }
    if(type_num == SUB && reg[rt] != 0x80000000){
        printf("SUB %02x == %02x\n", type_num, SUB);
        tmp = reg[rs] - reg[rt];
        if(reg[rs]>0 && reg[rt]<0 && tmp < 0){
            fprintf(err_file, "In cycle %d: Number Overflow\n", cycle);
            printf("In cycle %d: Number Overflow\n", cycle);
            //return_num = -1;
        }else if(reg[rs]<0 && reg[rt]>0 && tmp > 0){
            fprintf(err_file, "In cycle %d: Number Overflow\n", cycle);
            printf("In cycle %d: Number Overflow\n", cycle);
            //return_num = -1;
        }
    }
    if(type_num == SUB && reg[rt] == 0x80000000){
        tmp = reg[rs] + reg[rt];
        //printf("tmp = %08x, reg[rs] = %08x, reg[rt] = %08x\n", tmp, reg[rs], reg[rt]);
        if(reg[rs]>=0 && reg[rt]>=0 && tmp < 0){
            //printf("In cycle %d: Number Overflow\n", cycle);
            fprintf(err_file, "In cycle %d: Number Overflow\n", cycle);
            printf("In cycle %d: Number Overflow\n", cycle);
            //return_num = -1;
        }else if(reg[rs]<0 && reg[rt]<0 && tmp >=0){
            //printf("In cycle %d: Number Overflow\n", cycle);
            fprintf(err_file, "In cycle %d: Number Overflow\n", cycle);
            printf("In cycle %d: Number Overflow\n", cycle);
            //return_num = -1; // -1 keep going on
        }
    }
    if(return_num2 >= 0){
        return return_num2;
    }

    //printf("in r_func PC = 0x%08x\n", *PC);

    switch(type_num){
        case ADD:
            //printf("\nIt is ADD\n");
            //printf("ADD!\n");
            reg[rd] = add_ab(reg[rs], reg[rt]);

            break;
        case SUB:
            //printf("SUB!\n");
            if(reg[rt] != 0x80000000)reg[rd] = add_ab(reg[rs], -reg[rt]);
            else reg[rd] = add_ab(reg[rs], reg[rt]);

            break;
        case AND:
            reg[rd] = reg[rs] & reg[rt];
            break;
        case OR:
            reg[rd] = reg[rs] | reg[rt];;
            break;
        case XOR:
            reg[rd] = reg[rs] ^ reg[rt];
            break;
        case NOR:
            reg[rd] = ~(reg[rs] | reg[rt]);
            break;
        case NAND:
            reg[rd] = ~(reg[rs] & reg[rt]);
            break;
        case SLT:
            reg[rd] = (reg[rs] < reg[rt])? 1:0;
            break;
        case SLL:
            reg[rd] = reg[rt] << C;
            break;
        case SRL:
            reg[rd] = (unsigned int)reg[rt] >> C;
            break;
        case SRA:
            reg[rd] = reg[rt] >> C;
            break;
        case JR:
            *PC = reg[rs];
            break;
        default:
            break;
    }
    //printf("5: in r_func PC = 0x%08x\n", *PC);
    return 0;
}
