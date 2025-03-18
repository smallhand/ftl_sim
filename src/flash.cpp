
#include "flash.h"

//unsigned char* flash_arr = (unsigned char*)malloc(sizeof(unsigned char) * BLOCK_NUM * PAGE_PER_BLOCK * PAGE_SIZE);
unsigned char*** flash_arr = (unsigned char***)malloc(BLOCK_NUM * sizeof(unsigned char**));


void initFlash() {
    //memset(flash_arr, 0xFF, sizeof(unsigned char)* BLOCK_NUM * PAGE_PER_BLOCK * PAGE_SIZE);
    for (int i = 0; i < BLOCK_NUM; i++) {
        flash_arr[i] = (unsigned char**)malloc(PAGE_PER_BLOCK * sizeof(unsigned char*));
        //memset(flash_arr[i], 0xFF, PAGE_PER_BLOCK * sizeof(unsigned char));

        for (int j = 0; j < PAGE_PER_BLOCK; j++) {
            flash_arr[i][j] = (unsigned char*)malloc(PAGE_SIZE * sizeof(unsigned char));
            assert(flash_arr[i][j] != NULL);
            memset(flash_arr[i][j], 0xFF, PAGE_SIZE);
        }
    }
}

void freeFlash() {
    unsigned i, j;
    for (i = 0; i < BLOCK_NUM; i++) {
        for (j = 0; j < PAGE_PER_BLOCK; j++) {
            free(flash_arr[i][j]);
        }
    }
    for (i = 0; i < BLOCK_NUM; i++) {
        free(flash_arr[i]);
    }

    free(flash_arr);
}

void eraseBlock(unsigned block) {
    for (unsigned page = 0; page < PAGE_PER_BLOCK; page++) {
        memset(flash_arr[block][page], 0xFF, PAGE_SIZE);
    }
}

void progPage(unsigned p_adr, unsigned char* data) {
    // TODO: assert to check if valid state
    unsigned block = p_adr / PAGE_PER_BLOCK;
    unsigned page  = p_adr % PAGE_PER_BLOCK;

    //unsigned index = p_adr * PAGE_SIZE;

    memcpy(flash_arr[block][page], data, PAGE_SIZE);

    //memcpy((unsigned char*)&flash_arr[index], data, PAGE_SIZE);
}

void readPage(unsigned p_adr, unsigned char* data) {
    //unsigned index = p_adr * PAGE_SIZE;

    unsigned block = p_adr / PAGE_PER_BLOCK;
    unsigned page  = p_adr % PAGE_PER_BLOCK;

    memcpy(data, flash_arr[block][page], PAGE_SIZE);


    //memcpy(data, (unsigned char*)&flash_arr[index], PAGE_SIZE);
}
