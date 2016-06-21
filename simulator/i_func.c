#include <stdio.h>
#include "get_images.h"


//return halt?
int i_func(int cycle, FILE* err_file, int *PC, int *reg, int *mem, int type_num, unsigned int rs, unsigned int rt, int C){
    int return_num = -1; // 0 -> continue;  1->halt
    int tmp;
    unsigned int tmpu;
    unsigned int unstmp;

    *PC+=4;
    //write $0 error
    if(type_num != SW && type_num != SH && type_num != SB && type_num != BEQ && type_num != BNE){
        if(rt == 0){
            //printf("In cycle %d: Write $0 Error\n", cycle);
            fprintf(err_file, "In cycle %d: Write $0 Error\n", cycle);
            return_num = 0;
        }
    }
    //num overflow error
    if(type_num == ADDI || type_num == LW || type_num == LH || type_num == LHU || type_num == LB ||
            type_num == LBU || type_num == SW || type_num == SH || type_num == SB){
        tmp = reg[rs] + C;
        printf("tmp: %08x = %08x + %08x\n", tmp, reg[rs], C);
        //printf("tmp = %08x, reg[rs] = %08x, C = %08x", tmp, reg[rs], C);
        if(reg[rs]>=0 && C>=0 && tmp < 0){
            //printf("In cycle %d: Number Overflow\n", cycle);
            printf("a, In cycle %d: Number Overflow\n", cycle);
            fprintf(err_file, "In cycle %d: Number Overflow\n", cycle);
            //return_num = -1;
        }else if(reg[rs]<0 && C<0 && tmp >= 0){
            //printf("In cycle %d: Number Overflow\n", cycle);
            printf("b, In cycle %d: Number Overflow\n", cycle);
            fprintf(err_file, "In cycle %d: Number Overflow\n", cycle);
            //return_num = -1; // -1 keep going on
        }
    }

    //memory address overflow error
    if(type_num == LW || type_num == SW || type_num == SH || type_num == LHU || type_num == LH || type_num == SB
            || type_num == LBU  || type_num == LB){
        //printf("\n\nreg[rs]+C = %d\n\n", reg[rs]+C);
        int temp;
        if(type_num == LW || type_num == SW) temp = 3;
        else if(type_num == SH || type_num == LHU || type_num == LH) temp = 1;
        else if(type_num == SB || type_num == LBU  || type_num == LB) temp = 0;
        printf("address ovf: %d\n", reg[rs]+C);
        if(reg[rs]+C+temp >= 1024 || reg[rs]+C < 0 || reg[rs]+C+temp < 0 || reg[rs]+C >= 1024){
            //printf("YO!~~\n\n, reg[rs] = %d, C = %d, reg[rs]+C = %d\n", reg[rs], C, reg[rs]+C);
            //printf("In cycle %d: Address Overflow\n", cycle);
            fprintf(err_file, "In cycle %d: Address Overflow\n", cycle);
            return_num = 1;
        }
    }
    //misaligned error
    tmp = reg[rs]+C;
    if(type_num == LW || type_num == SW){
        if(tmp%4 != 0){
            //printf("In cycle %d: Misalignment Error\n", cycle);
            fprintf(err_file, "In cycle %d: Misalignment Error\n", cycle);
            return_num = 1;
        }
    }else if(type_num == SH || type_num == LHU || type_num == LH){
        if(tmp%2 != 0){
            //printf("In cycle %d: Misalignment Error\n", cycle);
            fprintf(err_file, "In cycle %d: Misalignment Error\n", cycle);
            return_num = 1;
        }
    }
    printf("return_num = %d\n", return_num);
    if(return_num >= 0){
        return return_num;
    }



    switch(type_num){
        case ADDI:
            reg[rt] = reg[rs] + C;
            break;
        case LW:
            reg[rt] = (mem[reg[rs] + C]<<24) + (mem[reg[rs] + C+1]<<16) + (mem[reg[rs] + C+2]<<8) + (mem[reg[rs] + C+3]);
            break;
        case LH:
            tmp = (mem[reg[rs] + C]<<8) + (mem[reg[rs] + C+1]);
            tmp = tmp << 16;
            reg[rt] = tmp >> 16;
            break;
        case LHU:
            tmpu = (mem[reg[rs] + C]<<8) + (mem[reg[rs] + C+1]);
            tmpu = tmpu << 16;
            reg[rt] = tmpu >> 16;
            break;
        case LB:
            tmp = mem[reg[rs] + C];
            tmp = tmp << 24;
            reg[rt] = tmp >> 24;
            break;
        case LBU:
            tmpu = mem[reg[rs] + C];
            tmpu = tmpu << 24;
            reg[rt] = tmpu >> 24;
            break;
        case SW:
            mem[reg[rs] + C] = reg[rt]>>24;
            tmp = reg[rt] & 0x00ff0000;
            mem[reg[rs] + C+1] = tmp>>16;
            tmp = reg[rt] & 0x0000ff00;
            mem[reg[rs] + C+2] = tmp>>8;
            tmp = reg[rt] & 0x000000ff;
            mem[reg[rs] + C+3] = tmp;
            break;
        case SH:
            tmp = reg[rt] & 0x0000ff00;
            mem[reg[rs] + C] = tmp>>8;
            tmp = reg[rt] & 0x000000ff;
            mem[reg[rs] + C+1] = tmp;
            break;
        case SB:
            mem[reg[rs] + C] = reg[rt] & 0x000000ff;
            break;
        case LUI:
            reg[rt] = (C << 16);
            //printf("\nLUI~  C = %08x, C<<16 = %08x\n", C, C<<16);
            break;
        case ANDI:
            unstmp = (C<<16);
            unstmp = (unstmp>>16);
            reg[rt] = reg[rs] & unstmp;
            break;
        case ORI:
            unstmp = (C<<16);
            unstmp = (unstmp>>16);
            reg[rt] = reg[rs] | unstmp;
            //printf("reg[rs] = 0x%08x  ; unstmp = %08x;  || = 0x%08x\n", reg[rs], unstmp, reg[rt]);
            break;
        case NORI:
            unstmp = (C<<16);
            unstmp = (unstmp>>16);
            reg[rt] = ~(reg[rs] | unstmp);
            break;
        case SLTI:
            reg[rt] = (reg[rs] < C)? 1:0;
            break;
        case BEQ:
            if(reg[rs] == reg[rt]) *PC += 4*C;
            break;
        case BNE:
            if(reg[rs] != reg[rt]) *PC += 4*C;
            break;
        default:
            break;
    }
    return 0;
}
