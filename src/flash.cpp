#include "flash.h"

unsigned char*** flash_arr = (unsigned char***)malloc(BLOCK_NUM * sizeof(unsigned char**));


void initFlash() {
    for (int i = 0; i < BLOCK_NUM; i++) {
        flash_arr[i] = (unsigned char**)malloc(PAGE_PER_BLOCK * sizeof(unsigned char*));

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
    unsigned block = p_adr / PAGE_PER_BLOCK;
    unsigned page  = p_adr % PAGE_PER_BLOCK;

    memcpy(flash_arr[block][page], data, PAGE_SIZE);

    //memcpy((unsigned char*)&flash_arr[index], data, PAGE_SIZE);
}

void readPage(unsigned p_adr, unsigned char* data) {
    unsigned block = p_adr / PAGE_PER_BLOCK;
    unsigned page  = p_adr % PAGE_PER_BLOCK;

    memcpy(data, flash_arr[block][page], PAGE_SIZE);
}
