/*
 * reconn.c
 *
 *  Created on: 2014年12月25日
 *      Author: LiJie
 */

#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include "log.h"
#include "wmpctrl.h"
#include "udphelp.h"
#include "wtpinfo.h"
#include "util.h"

extern int G_wmpsock;
extern struct sockaddr_in G_wmpaddr;


static int wmpWaitRecvAC(int fd);

static int wmpWaitReconnReq(void);
static int wmpWaitRcvAll(int to);

/*
 * 等待300s,每隔5s发送一次重连请求
 * 如果在这300s内接收到AC的重连回复,则切换状态到恢复状态
 * 否则,重启AP
 */
void wmpWait(int *state)
{
	int i = 0;
	for ( ; i < 60; ++i) {
		//  发送重连请求
		wmpWaitReconnReq();
		//  等待接收回复
		if (0 == wmpWaitRcvAll(5)) {
			//  接收到AC的重连回复
			*state = WTP_STATE_RECOV;
			logWarn("On %d RECONN-REQ, RECV AC reply");
			return;
		}
	}
	//  重启AP
	logError("Wait-State timeout, after 10s, will reboot!!!");
	sleep(5);
	logError("Now Reboot!!!");
	utilDoCMD("reboot");
}

/*
 *
 */
static int wmpWaitReconnReq(void)
{
	WMMSG msg;
	uint16_t seqnum = 0;
	uint8_t *apid = NULL;

	apid = (uint8_t *)wtpidGetBytes();
	seqnum = wtpInfoGetSEQNUM();
	if (wmmClearInit(&msg, apid, seqnum, WMM_CODE_RECONN_REQ)) {
		logError("Message head INIT error");
		return -1;
	}
	if (udpSend(G_wmpsock, msg.buff, msg.offset, &G_wmpaddr) < 1) {
		logError("sendto : %s", strerror(errno));
		return -1;
	}
	return 0;
}

/*
 *
 */
static int wmpWaitRcvAll(int to)
{
	int rc = 0;
	fd_set fset;
	struct timeval tv;
	int max_fd = G_wmpsock;

	tv.tv_sec = to;
	tv.tv_usec = 0;

	do {
		FD_ZERO(&fset);
		FD_SET(G_wmpsock, &fset);
		rc = select(max_fd + 1, &fset, NULL, NULL, &tv);
		if (rc > 0) {
			//  AC发来消息
			if (FD_ISSET(G_wmpsock, &fset)) {
				if (0 == wmpWaitRecvAC(G_wmpsock)) {
					return 0;
				}
			}
		}
	} while (rc > 0);

	return -1;
}


/*
 *
 */
static int wmpWaitRecvAC(int fd)
{
	WMMSG msg;
	struct sockaddr_in from;
	int len = 0;
	BUFF attr;
	uint32_t rescode = 0;

	memset(&msg, 0, sizeof(msg));
	memset(&from, 0, sizeof(from));
	len = udpRecv(fd, msg.buff, WMM_BUFF_SIZE, &from);
	if (len < 1) {
		logError("recvfrom : %s", strerror(errno));
		return -1;
	}
	msg.totlen = len;
	if (wmmPrepRecvd(&msg)) {
		logError("Preprocess message error");
		return -1;
	}
	if (WMM_CODE_RECONN_REP == msg.code && from.sin_addr.s_addr == G_wmpaddr.sin_addr.s_addr) {
		if (1 == wmmGetAttr(&msg, &attr)) {
			if (WMM_ATTR_RESCODE == attr.type) {
				if (wmmParseResCode(&attr, &rescode)) {
					logError("Parse RESULT-CODE error");
					return -1;
				} else {
					if (WMM_RESULT_CODE_OK == rescode) {
						return 0;
					}
				}
			}
		}
	}
	return -1;
}
