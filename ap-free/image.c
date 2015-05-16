/*
 * image.c
 *
 *  Created on: 2014年12月25日
 *      Author: LiJie
 */

#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include "log.h"
#include "wmpctrl.h"
#include "udphelp.h"
#include "wtpinfo.h"

extern int G_wmpsock;
extern struct sockaddr_in G_wmpaddr;

static struct img_ident S_img;


int wmpImageSetImageIdent(struct img_ident *img)
{
	if (!img) {
		logError("ARG error");
		return -1;
	}
	memset(&S_img, 0, sizeof(S_img));
	S_img.filesize = img->filesize;
	strncpy(S_img.filehash, img->filehash, 32);
	strncpy(S_img.name, img->name, 64);
	return 0;
}

int imageFileMD5Chk(char *name, const char *h)
{
	int rc = 0;
	FILE *fp = NULL;
	char *pathtemp = "/tmp/%s.md5";
	char path[1024] = { 0 };
	char md5str[128] = { 0 };
//	uint8_t md5[16] = { 0 };

	if (snprintf(path, sizeof(path), pathtemp, name) < 1) {
		logError("snprintf : %s", strerror(errno));
		return -1;
	}
	fp = fopen(path, "r");
	if (!fp) {
		logError("fopen \"%s\" : %s", path, strerror(errno));
		return -1;
	}
	if (fread(md5str, 32, 1, fp) < 1) {
		logError("fread \"%s\" : %s", path, strerror(errno));
		fclose(fp);
		return -1;
	}
	fclose(fp);
	logInfo("MD5 : %s", md5str);
#if 0
	rc = sscanf(md5str, "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X",
			&(md5[0]), &(md5[1]), &(md5[2]), &(md5[3]),
			&(md5[4]), &(md5[5]), &(md5[6]), &(md5[7]),
			&(md5[8]), &(md5[9]), &(md5[10]), &(md5[11]),
			&(md5[12]), &(md5[13]), &(md5[14]), &(md5[15]));
	if (rc != 16) {
		logError("sscanf : %s", strerror(errno));
		return -1;
	}
#endif
	if (strncasecmp(h, md5str, 32)) {
		logError("File \"%s\" MD5 %s[%s] is error, ", name, md5str, h);
		return -1;
	}
	return 0;
}

void wmpImage(int *state)
{
	int rc = 0;
	char *addr = (char *)wtpinfoGetACIPStr();
	char *dir = (char *)wtpInfoGetBoardName();
	char *file = S_img.name;
	char *imgdwn = "/sbin/imgdwn.sh %s %s %s";
	char *imgwr = "/sbin/imgwr.sh %s";
	char cmd[1024] = { 0 };
	WMMSG msg;
	BUFF attr;
	uint16_t seqnum = 0;
	uint8_t *apid = NULL;

	if (snprintf(cmd, sizeof(cmd), imgdwn, addr, dir, file) < 1) {
		logError("snprintf : %s", strerror(errno));
		rc = -1;
		goto out;
	}
	//  下载固件
	if (utilDoCMD(cmd)) {
		logError("Down firmware error");
		rc = -1;
		goto out;
	}
	//  对比固件MD5
	if (imageFileMD5Chk(file, S_img.filehash)) {
		logError("Image file \"%s\" MD5 hash is error");
		rc = -1;
		goto out;
	}
	//  写固件
	memset(cmd, 0, sizeof(cmd));
	if (snprintf(cmd, sizeof(cmd), imgwr, file) < 1) {
		logError("snprintf : %s", strerror(errno));
		rc = -1;
		goto out;
	}
	if (utilDoCMD(cmd)) {
		logError("Write firmware error");
		rc = -1;
	}
out :
	apid = (uint8_t *)wtpidGetBytes();
	seqnum = wtpInfoGetSEQNUM();
	wmmClearInit(&msg, apid, seqnum, WMM_CODE_IMGUP_REP);
	if (0 == rc) {
		//  报告AC固件处理完毕，准备重启
		wmmPackResCode(&attr, WMM_RESULT_CODE_OK);
	} else {
		//  报告AC固件处理出现错误，准备重启
		wmmPackResCode(&attr, WMM_RESULT_CODE_FAILED);
	}
	wmmPutAttrBuff(&msg, &attr);
	if (udpSend(G_wmpsock, msg.buff, msg.offset, &G_wmpaddr) < 1) {
		logError("sendto : %s", strerror(errno));
	}

	sleep(10);
	logError("Now Reboot!!!");
	utilDoCMD("reboot");
}
