/*
 * wtpid.c
 *
 *  Created on: 2015年1月6日
 *      Author: LiJie
 */


#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <uuid/uuid.h>

#include "log.h"
#include "wtpinfo.h"

#define WTPID_WRTMP_PATH	"/tmp/apid"

static char S_wtpidstr[128] = { 0 };
static uuid_t S_wtpid;

uint8_t uuid_enable[8][8] = {
		{0, 1, 0, 0, 0, 0, 1, 0},
		{0, 0, 1, 0, 1, 0, 0, 0},
		{0, 1, 0, 1, 0, 0, 0, 1},
		{0, 0, 0, 0, 1, 1, 0, 0},
		{0, 0, 1, 0, 0, 0, 0, 0},
		{0, 1, 0, 0, 0, 0, 1, 0},
		{0, 0, 1, 0, 0, 1, 0, 0},
		{0, 0, 0, 1, 0, 0, 1, 0},
};

struct wtp_uuid_block {
	//  标志字符LJ
	char LJ[2];
	//  UUID存储版本
	uint16_t version;
	//  随机偏移量，基数行优先，偶数列优先
	uint16_t offset;
};



int wtpidRead(const char *file, char *uuidstr)
{
	int i = 0;
	int j = 0;
	int n = 0;
	char row = 0;
	FILE *fp = NULL;
	uint16_t version = 0;
	uint8_t array[8][8];
	char buff[6] = {0};
	struct wtp_uuid_block *block = NULL;

	if (!file || !uuidstr) {
		logError("Args error");
		return -1;
	}

	fp = fopen(file, "r");
	if (!fp) {
		logError("fopen : %s", strerror(errno));
		return -1;
	}
	if (fread(buff, 6, 1, fp) < 1) {
		logError("fread : %s", strerror(errno));
		fclose(fp);
		return -1;
	}
	block = (struct wtp_uuid_block *)buff;
	if ('L' == block->LJ[0] && 'J' == block->LJ[1]) {
		if (1 == block->version && block->offset < 1024) {
			row = block->offset % 2;
			if (fseek(fp, (block->offset), SEEK_CUR)) {
				logError("fseek : %s", strerror(errno));
				fclose(fp);
				return -1;
			}
			for (i = 0; i < 8; ++i) {
				for (j = 0; j < 8; ++j) {
					if (fread(&(array[i][j]), 1, 1, fp) < 1) {
						fclose(fp);
						return -1;
					}
				}
			}
			for (i = 0; i < 8; ++i) {
				for (j = 0; j < 8; ++j) {
					if (1 == uuid_enable[i][j]) {
						if (1 == row) {
							S_wtpid[n++] = array[i][j];
						} else {
							S_wtpid[n++] = array[j][i];
						}
					}
				}
			}
			uuid_unparse_upper(S_wtpid, uuidstr);
			fclose(fp);
			return 0;
		}
	} else {
		logError("File Head error");
	}
	fclose(fp);
	return -1;
}


/*
 *
 */
int wtpidCreate(const char *file)
{
	int i = 0;
	int j = 0;
	int n = 0;
	uint8_t tmp = 0;
	uint8_t array[8][8];
	uint16_t version = 1;
	uint16_t offset = 0;
	char row = 0;
	FILE *fp = NULL;

	if (!file) {
		logError("Arg error");
		return -1;
	}

	fp = fopen(file, "w");
	if (!fp) {
		logError("fopen : %s", strerror(errno));
		return -1;
	}
	uuid_clear(S_wtpid);
	uuid_generate(S_wtpid);
	if (uuid_is_null(S_wtpid)) {
		logError("Create UUID error");
		return -1;
	}

	fp = fopen(file, "w");
	if (!fp) {
		logError("fopen : %s", strerror(errno));
		return -1;
	}
	srand(time(NULL));
	offset = (uint16_t)(rand() % 1024);
	row = offset % 2;
	//  标志
	if (fwrite("LJ", 2, 1, fp) < 1) {
		logError("fwrite : %s", strerror(errno));
		fclose(fp);
		return -1;
	}
	//  版本
	if (fwrite(&version, 2, 1, fp) < 1) {
		logError("fwrite : %s", strerror(errno));
		fclose(fp);
		return -1;
	}
	//  偏移量
	if (fwrite(&offset, 2, 1, fp) < 1) {
		logError("fwrite : %s", strerror(errno));
		fclose(fp);
		return -1;
	}
	for (i = 0; i < offset; ++i) {
		tmp = (uint8_t)(rand() % 256);
		if (fwrite(&tmp, 1, 1, fp) < 1) {
			logError("fwrite : %s", strerror(errno));
			fclose(fp);
			return -1;
		}
	}
	for (i = 0; i < 8; ++i) {
		for (j = 0; j < 8; ++j) {
			//  矩阵行优先
			if (1 == row) {
				//  写入有效值
				if (1 == uuid_enable[i][j]) {
					if (fwrite(&(S_wtpid[n++]), 1, 1, fp) < 1) {
						logError("fwrite : %s", strerror(errno));
						fclose(fp);
						return -1;
					}
				} else {
				// 写入随机值
					tmp = (uint8_t) (rand() % 256);
					if (fwrite(&tmp, 1, 1, fp) < 1) {
						logError("fwrite : %s", strerror(errno));
						fclose(fp);
						return -1;
					}
				}
			} else {
				//  矩阵列优先
				if (1 == uuid_enable[j][i]) {
					if (fwrite(&(S_wtpid[n++]), 1, 1, fp) < 1) {
						logError("fwrite : %s", strerror(errno));
						fclose(fp);
						return -1;
					}
				} else {
					tmp = (uint8_t) (rand() % 256);
					if (fwrite(&tmp, 1, 1, fp) < 1) {
						logError("fwrite : %s", strerror(errno));
						fclose(fp);
						return -1;
					}
				}
			}
		}
	}
	fclose(fp);
	return 0;
}


/*
 * 获取AP的UUID字符串
 */
const char *wtpidGetstr(void)
{
	return S_wtpidstr;
}

/*
 * 获取AP的UUID二进制
 */
const uint8_t *wtpidGetBytes(void)
{
	return S_wtpid;
}

static int wtpidWriteTmp(const char *s)
{
	FILE *fp = NULL;

	if (!s) {
		logError("ARG error");
		return -1;
	}
	fp = fopen(WTPID_WRTMP_PATH, "w");
	if (!fp) {
		logError("fopen : %s", strerror(errno));
		return -1;
	}
	if (fwrite(s, strlen(s), 1, fp) < 1) {
		logError("fwrite : %s", strerror(errno));
		fclose(fp);
		return -1;
	}
	fclose(fp);
	return 0;
}

/*
 *
 */
int wtpidInit(void)
{
	if (wtpidRead(WTPID_PATH, S_wtpidstr)) {
		if (wtpidCreate(WTPID_PATH)) {
			logError("Create WTP-ID error");
			return -1;
		}
		if (wtpidRead(WTPID_PATH, S_wtpidstr)) {
			logError("Read WTP-ID error");
			return -1;
		}
	}
	logInfo("WTP-ID : %s", S_wtpidstr);
#if 0
	wtpidWriteTmp(S_wtpidstr);
#endif
	return 0;
}
