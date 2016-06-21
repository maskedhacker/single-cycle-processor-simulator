#include <stdio.h>
#include <stdlib.h>
#include "get_images.h"
#define IIMAGE 1024
#define DIMAGE 1024
#define MEM 1024
unsigned int PC;
int reg[32];
INST* inst_mem[IIMAGE];
int mem[DIMAGE];
INST* if_id;
INST* id_ex;
INST* ex_mem;
INST* mem_wb;

void show_reg(FILE* output,unsigned int cycle);//show cycle, reg[], and PC



int main(){
    //initalize reg
    unsigned int i;
    INST* nop_inst = (INST*)malloc(sizeof(INST));
    nop_inst->rs = 0;
    nop_inst->capital_c = 0;
    nop_inst->rd = 0;
    nop_inst->rt = 0;
    nop_inst->type = R;
    nop_inst->r_type = SLL;

    for(i = 0; i < 32; i++) reg[i] = 0;
    for(i = 0; i < IIMAGE; i++) inst_mem[i] = (INST*)malloc(sizeof(INST));
    //initalize mem

    if_id = nop_inst;
    id_ex = nop_inst;
    ex_mem = nop_inst;
    mem_wb = nop_inst;


    get_iimage(&PC, inst_mem);
    get_dimage(&reg[29], mem);

    //define snapshot
    FILE* snapshot;
    FILE* err_file;
    snapshot = fopen("snapshot.rpt", "w");
    err_file = fopen("error_dump.rpt", "w");

    FILE* output = fopen("compare.txt", "w");

    INST *cur = cur = inst_mem[PC/4];
    i = 0;
    int halt = 0;
    //printf("ori_PC = 0x%x\n", PC);
    while(PC< 1024 && halt == 0){

        show_reg(snapshot, i);
        cur = inst_mem[PC/4];
        if(cur->type == JS && cur->js_type == HALT) break;


        if(cur->type == R) {
            //printf("R_type:  i = %ud; num = %d; r_type = 0x%02x; rs = %d; rt = %d; rd = %d; C = %x\n", i+1, (PC-ori_PC)/4, cur->r_type, cur->rs, cur->rt, cur->rd, cur->capital_c);
            printf("R_type: %d: r: %d, %d, %d, %d(%06x), %02x\n", i+1, cur->rs, cur->rt, cur->rd, cur->capital_c, cur->capital_c, cur->r_type);
            fprintf(output, "cycle %d: r: %d, %d, %d, %d(%06x), %02x\n", i+1, cur->rs, cur->rt, cur->rd, cur->capital_c, cur->capital_c, cur->r_type);
            halt = r_func(i+1, err_file, &PC, reg, mem, cur->r_type, cur->rs, cur->rt, cur->rd, cur->capital_c);
            //printf("PC = 0x%08x\n", PC);
        }
        else if(cur->type == I) {

            //printf("I_type:  i = %ud; num = %d; i_type = 0x%02x; rs = %d; rt = %d; C = %d \n", i+1, -(ori_PC-PC)/4, cur->i_type, cur->rs, cur->rt, cur->capital_c);
            printf("I_type:  %d: i: %d, %d, %d(%06x), %02x\n", i+1, cur->rs, cur->rt, cur->capital_c, cur->capital_c, cur->i_type);
            fprintf(output, "cycle %d: i: %d, %d, %d(%06x), %02x\n", i+1, cur->rs, cur->rt, cur->capital_c, cur->capital_c, cur->i_type);
            halt = i_func(i+1, err_file, &PC, reg, mem, cur->i_type, cur->rs, cur->rt, cur->capital_c);

        }
        else if(cur->type == JS) {

            //printf("JS_type:  i = %ud; num = %d; js_type = 0x%02x;C = 0x%x \n", i+1, -(ori_PC-PC)/4, cur->js_type, cur->capital_c);
            halt = js_func(i+1, err_file, &PC, reg, mem, cur->js_type, cur->capital_c);
            printf("%d: js: %d, %d(%06x), %02x\n", i+1, cur->rs, cur->capital_c, cur->capital_c, cur->js_type);
        }
        //printf("(PC-ori_PC)/4 = %ud\n", (PC-ori_PC)/4);
        printf("cycle %d: PC = 0x%08x\nhalt = %d\n", i+1, PC, halt);
        i++;

    }
    //printf("instr_num = %d\n", inst_num);


}



void show_reg(FILE* output, unsigned int cycle){
    fprintf(output, "cycle %u\n", cycle);
    int i;
    for(i = 0 ; i < 32; i++){
        fprintf(output, "$%02u: 0x%08X\n", i, reg[i]);
    }
    fprintf(output, "PC: 0x%08X\n\n\n", PC);
}

void show_mem(int d_mem_num){
    printf("Memory:\n");
    int i;
    for(i = 0 ; i < d_mem_num*4; i++){
        printf("mem%2d: 0x%02x\n", i, mem[i]);
    }
}
