/*
 * join.c
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

struct join_rep {
	uint32_t rescode;
	uint32_t echo;
	uint32_t imgup;
	struct img_ident img;
};


extern int G_wmpsock;
extern struct sockaddr_in G_wmpaddr;

static int joinParseRep(WMMSG *msg, struct join_rep *rep);
static int joinHandleRep(struct join_rep *rep);
static int wtpHandleWiFi(BUFF *msg);

void wmpJoin(int *state)
{
	WMMSG reqmsg;
	WMMSG repmsg;
	BUFF attr;
	struct sockaddr_in from;
	int len = 0;
	uint16_t seqnum = 0;
	uint8_t *apid = NULL;
	struct join_rep rep;
	uint8_t *p = NULL;

	if (G_wmpsock > 0) {
		udpCloseConnect(G_wmpsock);
	}
	G_wmpsock = udpNewNonblockLongConnect(NULL, NULL, wtpinfoGetACIPStr(), WMP_CTRL_PORT_STR);
	if (G_wmpsock < 0) {
		logError("New UDP long connect error");
		*state = WTP_STATE_SULK;
		return;
	}

	apid = (uint8_t *)wtpidGetBytes();
	seqnum = wtpInfoGetSEQNUM();
	if (wmmClearInit(&reqmsg, apid, seqnum, WMM_CODE_JOIN_REQ)) {
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
	len = udpTimeRecv(G_wmpsock, repmsg.buff, WMM_BUFF_SIZE, &from, 2000);
	if (len < 1) {
		logError("RECV message error");
		*state = WTP_STATE_SULK;
		return;
	}
	if (from.sin_addr.s_addr != G_wmpaddr.sin_addr.s_addr) {
		logError("Message source error");
		*state = WTP_STATE_SULK;
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
	if (WMM_CODE_JOIN_REP == repmsg.code) {
		//  解析JOIN响应消息
		memset(&rep, 0, sizeof(rep));
		if (joinParseRep(&repmsg, &rep)) {
			logError("Parse response message error");
			*state = WTP_STATE_SULK;
			return;
		}
		// 处理消息
		if (joinHandleRep(&rep)) {
			logError("Handle response message error");
			*state = WTP_STATE_SULK;
			return;
		}
		if (1 == rep.imgup) {
			*state = WTP_STATE_IMAGE;
		} else {
			*state = WTP_STATE_RUN;
		}
	}
}


static int joinParseRep(WMMSG *msg, struct join_rep *rep)
{
	int rc = 0;
	BUFF attr;

	for ( ; ; ) {
		memset(&attr, 0, sizeof(attr));
		rc = wmmGetAttr(msg, &attr);
		if (1 == rc) {
			switch (attr.type) {
			case WMM_ATTR_RESCODE :
				if (wmmParseResCode(&attr, &(rep->rescode))) {
					logError("Parse Result-Code error");
					return -1;
				}
				break;
			case WMM_ATTR_ECHO:
				if (wmmParseEcho(&attr, &(rep->echo))) {
					logError("Parse ECHO-TIME error");
					return -1;
				}
				break;
			case WMM_ATTR_WIFICFG:
				if (wtpHandleWiFi(&attr)) {
					logError("Parse handle WiFi-CFG error");
					return -1;
				}
				break;
			default:
				logError("Unknown message attribute type %u, length %u", attr.type, attr.len);
				break;
			}
		} else {
			if (0 == rc) {
				break;
			} else {
				logError("Get attribute error");
			}
		}
	}
	if (msg->offset != msg->totlen) {
		logError("JOIN-REQ message exist unknown attribute length %d", (msg->totlen - msg->offset));
	}
	if (rep->rescode != WMM_RESULT_CODE_OK) {
		logError("Join failed");
		rc = -1;
	}
	return rc;
}

static int joinHandleRep(struct join_rep *rep)
{
	//  设置ECHO
	wtpInfoSetEcho(rep->echo);
	//  打开WiFi
	if (wtpWiFiOpen()) {
		logError("Open WiFi error");
		return -1;
	}
	return 0;
}

/*
 *
 */
static int wtpHandleWiFi(BUFF *msg)
{
	int rc = 0;
	struct wifi_cfg wifi;
	BUFF attr;

	memset(&wifi, 0, sizeof(wifi));
	//  解析并生成相应的配置文件
	if (wmmParseWIFI(msg, &wifi)) {
		logError("Parse WiFi error");
		return -1;
	}
	rc = wtpWiFiWirteFile(wifi.wlanindex, wifi.encrypt, "g", wifi.channel, wifi.ssidsupp,
			wifi.ssid, wifi.key);
	if (rc < 0) {
		logError("Write WiFi configuration error");
		return -1;
	}
	return 0;
}
