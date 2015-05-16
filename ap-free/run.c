/*
 * echo.c
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


static int wmpRunHandleACMsg(int fd);
static int wmmRebootReq(WMMSG *msg);

static int wmmEchoReq(void);
static int wmmEchoRep(WMMSG *msg);

//  ECHO计数
static int S_echonum = 0;


/*
 *
 */
void wmpRun(int *state)
{
	int rc = 0;
	int addrlen = sizeof(struct sockaddr_in);
	fd_set fset;
	struct timeval tv;
	int max_fd = G_wmpsock;
	uint32_t echo = 0;

	echo = wtpInfoGetEcho();
	tv.tv_sec = echo;
	tv.tv_usec = 0;

	do {
		FD_ZERO(&fset);
		FD_SET(G_wmpsock, &fset);
		rc = select(max_fd + 1, &fset, NULL, NULL, &tv);
		if (rc > 0) {
			//  AC发来消息
			if (FD_ISSET(G_wmpsock, &fset)) {
				wmpRunHandleACMsg(G_wmpsock);
			}
		}
	} while (rc > 0);
	//  检查AC是否回复ECHO
	if (S_echonum == WTP_ECHO_MAX) {
		logError("This WTP lost contact");
		S_echonum = 0;
		*state = WTP_STATE_WAIT;
		return;
	}
	//  超时，发送ECHO
	if (0 == rc) {
		rc = wmmEchoReq();
		if (rc) {
			logError("Send ECHO-REQ error");
			*state = WTP_STATE_WAIT;
			S_echonum = 0;
		} else {
			*state = WTP_STATE_RUN;
			S_echonum++;
		}
	} else {
		//  出错
		*state = WTP_STATE_WAIT;
		S_echonum = 0;
	}
}

/*
 * 处理AC发来的消息
 */
static int wmpRunHandleACMsg(int fd)
{
	WMMSG msg;
	struct sockaddr_in from;
	int len = 0;

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
	logDebug("AC message code=%d, length=%d", msg.code, len);
	switch (msg.code) {
	case WMM_CODE_ECHO_REP :
		wmmEchoRep(&msg);
		break;
	case WMM_CODE_REBOOT_REQ :
		if (wmmRebootReq(&msg)) {
			logError("Handle REBOOT-REQ error");
		} else {
			sleep(2);
			utilDoCMD("reboot");
		}
		break;
	default:
		logError("Unknown WMP message code=%d, length=%d", msg.code, msg.totlen);
		break;
	}
	return 0;
}

/*
 *
 */
static int wmmEchoRep(WMMSG *msg)
{
	S_echonum = 0;
	if (msg->offset != msg->totlen) {
		logError("ECHO-REP message exist unknown attribute length %d", (msg->totlen - msg->offset));
	}
	logDebug("Hand ECHO-REP message");
	return 0;
}

/*
 * 发送ECHO请求
 */
static int wmmEchoReq(void)
{
	WMMSG reqmsg;
	uint16_t seqnum = 0;
	uint8_t *apid = NULL;

	apid = (uint8_t *)wtpidGetBytes();
	seqnum = wtpInfoGetSEQNUM();
	if (wmmClearInit(&reqmsg, apid, seqnum, WMM_CODE_ECHO_REQ)) {
		logError("Message head INIT error");
		return -1;
	}
	if (udpSend(G_wmpsock, reqmsg.buff, reqmsg.offset, &G_wmpaddr) < 1) {
		logError("sendto : %s", strerror(errno));
		return -1;
	}
	logDebug("Send ECHO-REQ message");
	return 0;
}

/*
 * 处理REBOOT请求
 */
static int wmmRebootReq(WMMSG *msg)
{
	WMMSG reqmsg;
	uint16_t seqnum = 0;
	uint8_t *apid = NULL;

	if (msg->offset != msg->totlen) {
		logError("REBOOT-REQ message exist unknown attribute length %d", (msg->totlen - msg->offset));
	}

	apid = (uint8_t *) wtpidGetBytes();
	seqnum = wtpInfoGetSEQNUM();
	if (wmmClearInit(&reqmsg, apid, seqnum, WMM_CODE_REBOOT_REP)) {
		logError("Message head INIT error");
		return -1;
	}
	if (udpSend(G_wmpsock, reqmsg.buff, reqmsg.offset, &G_wmpaddr) < 1) {
		logError("sendto : %s", strerror(errno));
		return -1;
	}
	logDebug("Send REBOOT-REP message");
	return 0;
}
