#include "ftl.h"
#include "flash.h"

#define GC_THRSH	(((BLOCK_NUM - 4) * PAGE_PER_BLOCK) - 1) // 32640 - 1 = 32639

unsigned *l2p_table = (unsigned *)malloc(sizeof(unsigned) * PAGE_PER_BLOCK * USER_BLOCK_NUM);
unsigned *p2l_table = (unsigned *)malloc(sizeof(unsigned) * PAGE_PER_BLOCK * BLOCK_NUM);
/*
	********************************************************
			Private functions
	********************************************************
*/
static unsigned page_cnt = 0; // for assign physical page adr
static unsigned prog_page_cnt = 0; // valid + invalid
static unsigned erase_cnt[BLOCK_NUM] = { 0 };
static unsigned block_cnt[BLOCK_NUM] = { 0 };  // to record block is written
INVALID_TABLE invalid_tbl[BLOCK_NUM];

static unsigned findYoungBlock() {
	unsigned min_cnt = UINT_MAX;
	unsigned i, block;

	for (i = 0; i < BLOCK_NUM; i++) {
		if ((block_cnt[i] == 0) && (erase_cnt[i] < min_cnt)) {
			block = i;
			min_cnt = erase_cnt[i];
		}
	}
	// TODO: check this block is empty or not
	assert(block_cnt[block] == 0);

	return block;
}

static unsigned findGCBlock() {
	unsigned max_invalid = 0;
	unsigned i;

	for (i = 1; i < BLOCK_NUM; i++) {
        if (invalid_tbl[i].invalid_size > invalid_tbl[max_invalid].invalid_size) {
            max_invalid = i;
		}
	}

	return max_invalid;
}

static void updatePageTable(unsigned lca) {
	if ((page_cnt % PAGE_PER_BLOCK) == 0) {
		page_cnt = findYoungBlock() * PAGE_PER_BLOCK;
	}
	l2p_table[lca] = page_cnt;
	p2l_table[page_cnt] = lca;
	page_cnt += 1;
}

static void doErase(unsigned block) {
	eraseBlock(block);
	erase_cnt[block] += 1;
	prog_page_cnt -= PAGE_PER_BLOCK;
	block_cnt[block] -= PAGE_PER_BLOCK;

	invalid_tbl[block].invalid_size = 0;
	memset(invalid_tbl[block].invalid_page, 0x0, PAGE_PER_BLOCK * sizeof(unsigned char));
}

static void GC(unsigned src_block, unsigned tar_block) {
	unsigned src_page = src_block * PAGE_PER_BLOCK;
	unsigned tar_page = tar_block * PAGE_PER_BLOCK;

	unsigned char data[PAGE_SIZE];
	unsigned lca, block;

	while (src_page < (src_block + 1) * PAGE_PER_BLOCK) {
        if (invalid_tbl[src_block].invalid_page[src_page % PAGE_PER_BLOCK] == 0) { // valid data
            readPage(src_page, data);
            progPage(tar_page, data);
            prog_page_cnt += 1;

            block = tar_page / PAGE_PER_BLOCK;
            block_cnt[block] += 1;

            lca = p2l_table[src_page];

            l2p_table[lca] = tar_page;
			p2l_table[tar_page] = lca;

			//p2l_table[src_page] = 0xFFFFFFFF;

			tar_page += 1;
			assert((tar_page % PAGE_PER_BLOCK) != 0);
        }
		src_page += 1;
	}

	doErase(src_block);
	page_cnt = tar_page;
}

static void checkGC() {
	unsigned src, tar;
	while (prog_page_cnt > GC_THRSH) {
		// src block: most invalid pages
		// tar block: youngest
		src = findGCBlock();
		tar = findYoungBlock();

		GC(src, tar);
	}
}

/*
	********************************************************
			Public functions
	********************************************************
*/
void initFTL() {
	memset(l2p_table, 0xFF, sizeof(unsigned) * PAGE_PER_BLOCK * USER_BLOCK_NUM);
	memset(p2l_table, 0xFF, sizeof(unsigned) * PAGE_PER_BLOCK * BLOCK_NUM);
	memset(&invalid_tbl, 0, sizeof(INVALID_TABLE));

	// TODO: Erase whole pages first
}

void freeFTL() {
    free(l2p_table);
    free(p2l_table);
}


void writeData(unsigned lba, unsigned sec_cnt, unsigned char* source) {
	unsigned start_lba = lba; 
	unsigned end_lba = lba + sec_cnt; 
	unsigned char* data = source;

	// 1. convert lba to logical page
	unsigned l_adr = lba / SECTOR_PER_PAGE;
	unsigned write_sectors;
	unsigned p_adr, block;

	//unsigned char *page_data = (unsigned char*)calloc(PAGE_SIZE, sizeof(unsigned char));
	unsigned char page_data[PAGE_SIZE];
	unsigned pos;

	if ((lba + sec_cnt) > USER_SECTOR_NUM) {
		printf("space is not enough\n");
		return;
	}

	while (start_lba < end_lba) {
        // 2. look up l2p table to check data already exists or not
		//    if exists, a. read it, and concate with source, then write to new page;
		//				 b. invalid original p_page
		//       not exists, write to a new page directly
		memset(page_data, 0x0, PAGE_SIZE);
        p_adr = l2p_table[l_adr];

		write_sectors = (l_adr + 1) * SECTOR_PER_PAGE - start_lba;
		if (write_sectors > (end_lba - start_lba)) {
			write_sectors = end_lba - start_lba;
		}

        if (l2p_table[l_adr] != INVALID_PAGE) { // overwrite case
			block = p_adr / PAGE_PER_BLOCK;
			invalid_tbl[block].invalid_page[p_adr % PAGE_PER_BLOCK] = 1;
			invalid_tbl[block].invalid_size += 1;

			readPage(p_adr, page_data);
			pos = start_lba % SECTOR_PER_PAGE;
			memcpy((unsigned char*)(page_data + pos * SECTOR_SIZE), data, write_sectors * SECTOR_SIZE);

			updatePageTable(l_adr);
        }
		else {
			updatePageTable(l_adr);
            memcpy(page_data, data, write_sectors * SECTOR_SIZE);
		}
		p_adr = l2p_table[l_adr]; // must look up again to refresh p_adr

        progPage(p_adr, page_data);
		prog_page_cnt += 1;

		block = p_adr / PAGE_PER_BLOCK;
		block_cnt[block] += 1;

		start_lba += write_sectors;
		l_adr = start_lba / SECTOR_PER_PAGE;

		data = data + write_sectors * SECTOR_SIZE;

		checkGC();
	}
}

void readData(unsigned lba, unsigned sec_cnt, unsigned char* source) {
	unsigned l_adr = lba / SECTOR_PER_PAGE;
	unsigned p_adr = l2p_table[l_adr];
	unsigned char page_data[PAGE_SIZE];

	unsigned pos = lba % SECTOR_PER_PAGE;
	unsigned read_sectors = SECTOR_PER_PAGE - pos;

	unsigned start = lba;
	unsigned end   = lba + sec_cnt;
	unsigned total_sectors = 0;

	while (start < end) {
		l_adr = start / SECTOR_PER_PAGE;
		p_adr = l2p_table[l_adr];
        if ((p_adr == INVALID_PAGE) || (sec_cnt == 0)) { // read before write || read no data
            *source = NULL;
            return;
        }
		
        readPage(p_adr, page_data);

		read_sectors = (l_adr + 1) * SECTOR_PER_PAGE - start;
		pos = start % SECTOR_PER_PAGE;
		if (read_sectors > end - start) {
			read_sectors = end - start;
		}

		memcpy(source + total_sectors * SECTOR_SIZE, page_data + pos * SECTOR_SIZE , read_sectors * SECTOR_SIZE);
		total_sectors += read_sectors;
		start += read_sectors;
	}
}
