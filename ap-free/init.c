/*
 * init.c
 *
 *  Created on: 2014年12月25日
 *      Author: LiJie
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include "log.h"
#include "wmp.h"
#include "wmpctrl.h"
#include "wtpinfo.h"

extern struct sockaddr_in G_wmpaddr;

static int wmpInitACAddr(void);
static int wmpInitAPId(void);
static int wmpInitBoardName(void);

/*
 *
 */
void wmpInit(int *state)
{
	int sulktm = 0;

	if (wmpInitAPId()) {
		logError("INIT WTP-ID error");
		exit(1);
	}
	if (wmpInitACAddr()) {
		logError("INIT AC-ADDR error");
		exit(1);
	}
	if (wmpInitBoardName()) {
		logError("INIT WTP-BOARD-NAME error");
		exit(1);
	}

	sulktm = random() % 10;
	logDebug("Now will sleep %ds to DSCV-State", sulktm);
	sleep(sulktm);
	*state = WTP_STATE_DSCV;
}

static int wmpInitBoardName(void)
{
#if 0
	char *file = "/tmp/sysinfo/board_name";
	FILE *fp = NULL;
	char buf[256] = { 0 };
	int len = 0;
	fp = fopen(file, "r");
	if (!fp) {
		logError("fopen : %s", strerror(errno));
		return -1;
	}
	if (fread(&buf, sizeof(char), sizeof(buf), fp) < 1) {
		logError("fread : %s %s", strerror(errno), buf);
		fclose(fp);
		return -1;
	}
	fclose(fp);
#endif
	char *buf = "PROTOTEST";
	int len = strlen(buf);

	return wtpInfoSetBoardName(buf, len);
}

/*
 *
 */
static int wmpInitACAddr(void)
{
	FILE *fp = NULL;
	char buff[128] = { 0 };
	uint32_t acaddr = 0;
	uint8_t *p = NULL;

	fp = fopen(WTPACADDR_PATH, "r");
	if (!fp) {
		logError("fopen : %s", strerror(errno));
		return -1;
	}
	if (fread(&acaddr, sizeof(uint32_t), 1, fp) < 1) {
		logError("fread : %s\n", strerror(errno));
		fclose(fp);
		return -1;
	}
	fclose(fp);

	p = (uint8_t *)&acaddr;
	if (snprintf(buff, sizeof(buff), "%u.%u.%u.%u", p[0], p[1], p[2], p[3]) < 1) {
		logError("snprintf : %s", strerror(errno));
		return -1;
	}
	if (0 == acaddr || 0xFFFFFFFF == acaddr) {
		logError("AC address %s is invalid value", buff);
		return -1;
	}
	logDebug("AC address %s", buff);
	G_wmpaddr.sin_family = AF_INET;
	G_wmpaddr.sin_addr.s_addr = acaddr;
	G_wmpaddr.sin_port = htons(WMP_CTRL_PORT);
	wtpinfoSetACIPStr(buff);
	return 0;
}

/*
 *
 */
static int wmpInitAPId(void)
{
	if (wtpidInit()) {
		logError("Get WTP-ID error");
		return -1;
	}
	return 0;
}
