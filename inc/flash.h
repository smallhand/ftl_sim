//#pragma once

#ifndef FLASH_H
#define FLASH_H
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#define BLOCK_NUM               (1024)
#define PAGE_PER_BLOCK          (32)
#define INVALID_PAGE            (0xFFFFFFFF)
// Total page# = 1024 * 32 = 32768 

#define PAGE_SIZE               (4096)
#define SECTOR_SIZE             (512)
#define SECTOR_PER_PAGE         (PAGE_SIZE / SECTOR_SIZE)

#define USER_BLOCK_NUM          (1000)
#define USER_SECTOR_NUM         (USER_BLOCK_NUM * PAGE_PER_BLOCK * SECTOR_PER_PAGE) // 256000

//extern unsigned char*flash_arr;
extern unsigned char*** flash_arr;

void initFlash();
void freeFlash();
void eraseBlock(unsigned block);
void progPage(unsigned p_adr, unsigned char* data);
void readPage(unsigned p_adr, unsigned char* data);


#endif // !FLASH_H

