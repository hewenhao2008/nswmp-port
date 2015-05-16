/*
 * dscv.c
 *
 *  Created on: 2014年12月24日
 *      Author: LiJie
 */


#include <stdio.h>
#include <netinet/in.h>
#include "log.h"
#include "wmpctrl.h"
#include "udphelp.h"
#include "wtpinfo.h"

extern int G_wmpsock;
extern struct sockaddr_in G_wmpaddr;

void wmpDscv(int *state)
{
	WMMSG reqmsg;
	WMMSG repmsg;
	BUFF attr;
	struct sockaddr_in from;
	int len = 0;
	uint16_t seqnum = 0;
	uint8_t *apid = NULL;

	if (G_wmpsock > 0) {
		udpCloseConnect(G_wmpsock);
	}
	G_wmpsock = udpNewConnect(0, 3000);
	if (G_wmpsock < 1) {
		logError("New UDP connect error");
		*state = WTP_STATE_SULK;
		return;
	}
	apid = (uint8_t *)wtpidGetBytes();
	seqnum = wtpInfoGetSEQNUM();
	if (wmmClearInit(&reqmsg, apid, seqnum, WMM_CODE_DSCV_REQ)) {
		logError("Message head INIT error");
		*state = WTP_STATE_SULK;
		return;
	}
	wmmPackBoard(&attr, WTP_FIRMWARE_MAJOR_VERSION, WTP_FIRMWARE_MINOR_VERSION, wtpInfoGetBoardName());

	if (wmmPutAttrBuff(&reqmsg, &attr)) {
		logError("Put attribute error");
		*state = WTP_STATE_SULK;
		return;
	}
	if (udpSend(G_wmpsock, reqmsg.buff, reqmsg.offset, &G_wmpaddr) < 1) {
		logError("sendto : %s", strerror(errno));
		*state = WTP_STATE_SULK;
		return;
	}
	memset(&repmsg, 0, sizeof(repmsg));
	len = udpRecv(G_wmpsock, repmsg.buff, WMM_BUFF_SIZE, &from);
	if (len < 1) {
		logError("recvfrom : %s", strerror(errno));
		*state = WTP_STATE_SULK;
		return;
	}
	if (from.sin_addr.s_addr != G_wmpaddr.sin_addr.s_addr) {
		logError("Message source is error");
		*state = WTP_STATE_SULK;
		return;
	}
	repmsg.totlen = len;
	if (wmmPrepRecvd(&repmsg)) {
		logError("Preprocess message error");
		*state = WTP_STATE_SULK;
		return;
	}
	if (repmsg.seqnum != seqnum) {
		logError("Response message SEQ-NUM %d don't match %d", repmsg.seqnum, seqnum);
		*state = WTP_STATE_SULK;
		return;
	}
	if (WMM_CODE_DSCV_REP == repmsg.code) {
		*state = WTP_STATE_JOIN;
	} else {
		logError("Response message code %d don't match %d", repmsg.code, WMM_CODE_DSCV_REP);
		*state = WTP_STATE_SULK;
	}
}
