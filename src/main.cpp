#include <iostream>
#include <stdio.h>

#include "ftl.h"
#include "flash.h"

int main(int argc, char* argv[])
{
    srand(3);
    unsigned char* data = (unsigned char*)calloc(USER_SECTOR_NUM * SECTOR_SIZE, sizeof(unsigned char));
    unsigned char* mirror_data = (unsigned char*)calloc(USER_SECTOR_NUM * SECTOR_SIZE, sizeof(unsigned char));
    unsigned int i, j;

    unsigned lba, sec_cnt;
    int result;

    memset(data, 0x0, USER_SECTOR_NUM * SECTOR_SIZE * sizeof(unsigned char));
    memset(mirror_data, 0x0, USER_SECTOR_NUM * SECTOR_SIZE * sizeof(unsigned char));

    initFlash();
    initFTL();

    // seq write
    unsigned start = 0;
    unsigned tmp;
    for (i = 0; i < 5; i++) { // re-write 5 rounds
        lba = 0;
        tmp = 0;
        while (lba < USER_SECTOR_NUM) {
            sec_cnt = rand() % 32;
            //sec_cnt = 10;
            if (sec_cnt > USER_SECTOR_NUM - lba){
                sec_cnt = USER_SECTOR_NUM - lba;
            }
            j = 0;
            while (j < SECTOR_SIZE * sec_cnt) {
                data[tmp++] = (tmp % 26) + 'a';
                j++;
            }
            //writeData(lba, sec_cnt, &data[lba * SECTOR_SIZE]);
            writeData(lba, sec_cnt, data + lba * SECTOR_SIZE);
            lba += sec_cnt;
        }
    }

    lba = 0;
    sec_cnt = 20;
    // seq read. and compare to data
    while (lba < USER_SECTOR_NUM) {
        //readData(lba, sec_cnt, mirror_data + lba * SECTOR_SIZE);
        sec_cnt = rand() % 32;
        if (sec_cnt > USER_SECTOR_NUM - lba){
            sec_cnt = USER_SECTOR_NUM - lba;
        }

        readData(lba, sec_cnt, mirror_data);
        result = memcmp(data + lba * SECTOR_SIZE, mirror_data, sec_cnt * SECTOR_SIZE);
        assert(result == 0);

        lba += sec_cnt;
    }
    printf("Compare successfully");

    freeFTL();
    freeFlash();
}

