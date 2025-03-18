//#pragma once
#ifndef FTL_H
#define FTL_H

#include <stdlib.h> // malloc
#include <string.h> // memset
#include <assert.h>
#include "flash.h"

extern unsigned *l2p_table;

void initFTL();
void freeFTL();
void writeData(unsigned lba, unsigned sec_cnt, unsigned char* source);
void readData(unsigned lba, unsigned sec_cnt, unsigned char* source);
//void mirror(unsigned char* mirror_data, unsigned cmp_size);

struct invalid_table {
    unsigned char invalid_page[PAGE_PER_BLOCK];
    unsigned int  invalid_size;
};

typedef struct invalid_table INVALID_TABLE, * INVALID_TABLE_PTR;

#endif
