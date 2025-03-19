#include <stdio.h>

#include "ftl.h"
#include "flash.h"

static void randGenData(unsigned char* data, unsigned sz) {
    unsigned i;
    for (i = 0; i < sz; i++) {
        data[i] = rand() % 255;
    }
}

static void readWholeDisk(unsigned char* data) {
    unsigned lba = 0;
    unsigned sec_cnt;

    while (lba < USER_SECTOR_NUM) {
        sec_cnt = rand() % 32;
        if (sec_cnt > USER_SECTOR_NUM - lba){
            sec_cnt = USER_SECTOR_NUM - lba;
        }
        readData(lba, sec_cnt, data + lba * SECTOR_SIZE);
        lba += sec_cnt;
    }
}

int main(int argc, char* argv[])
{
    srand(2);
    unsigned char* data = (unsigned char*)calloc(USER_SECTOR_NUM * SECTOR_SIZE, sizeof(unsigned char));
    unsigned char* mirror_data = (unsigned char*)calloc(USER_SECTOR_NUM * SECTOR_SIZE, sizeof(unsigned char));
    unsigned int i;
    unsigned test_round;

    unsigned lba, sec_cnt;
    int result;

    memset(data, 0x0, USER_SECTOR_NUM * SECTOR_SIZE * sizeof(unsigned char));
    memset(mirror_data, 0x0, USER_SECTOR_NUM * SECTOR_SIZE * sizeof(unsigned char));

    initFlash();
    initFTL();

    // seq write
    unsigned tmp;
    test_round = 5;
    for (i = 0; i < test_round; i++) { // re-write 5 rounds
        lba = 0;
        tmp = 0;
        while (lba < USER_SECTOR_NUM) {
            sec_cnt = rand() % 32;
            //sec_cnt = 10;
            if (sec_cnt > USER_SECTOR_NUM - lba){
                sec_cnt = USER_SECTOR_NUM - lba;
            }
            randGenData(data + (lba * SECTOR_SIZE), sec_cnt * SECTOR_SIZE);

            //writeData(lba, sec_cnt, &data[lba * SECTOR_SIZE]);
            writeData(lba, sec_cnt, data + lba * SECTOR_SIZE);
            lba += sec_cnt;
        }
    }

    // read then compare to written data
    lba = 0;
    readWholeDisk(mirror_data);

    result = memcmp(data, mirror_data, USER_SECTOR_NUM);
    assert(result == 0);
    printf("Sequential write pass\n");

    // random write
    test_round = 100000;
    i = 0;
    while (i++ < test_round) {
        sec_cnt = rand() % 32;
        lba = rand() % USER_SECTOR_NUM;

        if (sec_cnt > USER_BLOCK_NUM - lba) {
            sec_cnt = USER_BLOCK_NUM - lba;
        }

        randGenData(data + (lba * SECTOR_SIZE), sec_cnt * SECTOR_SIZE);
        writeData(lba, sec_cnt, data + lba * SECTOR_SIZE);
    }
    // read whole space and compare to written data 
    readWholeDisk(mirror_data);

    result = memcmp(data, mirror_data, USER_SECTOR_NUM);
    assert(result == 0);
    printf("Random write pass\n");

    freeFTL();
    freeFlash();
}

