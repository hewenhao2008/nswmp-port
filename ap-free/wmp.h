/*
 * wmm.h
 *
 *  Created on: 2014年12月16日
 *      Author: LiJie
 */

#ifndef __WTP_MGR_MSG_H__
#define __WTP_MGR_MSG_H__

#include <stdint.h>
#include <netinet/in.h>
#include "buff.h"

#define WMM_VERSION		1

#define WMP_CTRL_PORT	10086
#define WMP_USER_PORT	10087

#define WMP_CTRL_PORT_STR	"10086"
#define WMP_USER_PORT_STR	"10087"

/*
 * 加密type值
 * 0 - 消息不加密，明文传输
 * 1 - 加密协商阶段
 * 2 - 加密消息传输阶段
 */
#define WMM_TYPE_CLEAR	0
#define	WMM_TYPE_NEGO	1
#define WMM_TYPE_STL	2

//  WMM Code
#define	WMM_CODE_SEKEY_REQ		1
#define	WMM_CODE_SEKEY_REP		2

#define WMM_CODE_DSCV_REQ		3
#define WMM_CODE_DSCV_REP		4

#define WMM_CODE_JOIN_REQ		5
#define WMM_CODE_JOIN_REP		6

#define WMM_CODE_ECHO_REQ		7
#define WMM_CODE_ECHO_REP		8

#define WMM_CODE_REBOOT_REQ		9
#define WMM_CODE_REBOOT_REP		10

#define WMM_CODE_RECONN_REQ		11
#define WMM_CODE_RECONN_REP		12

#define WMM_CODE_RECOV_REQ		13
#define WMM_CODE_RECOV_REP		14

#define WMM_CODE_IMGUP_REQ		15
#define WMM_CODE_IMGUP_REP		16

#define WMM_CODE_NEWUSR_REQ		50
#define WMM_CODE_NEWUSR_REP		51

#define WMM_CODE_USRACCR_REQ	52
#define WMM_CODE_USRACCR_REP	53

#define WMM_CODE_USRLEFT_REQ	54
#define WMM_CODE_USRLEFT_REP	55

#define WMM_CODE_USRDROP_REQ	56
#define WMM_CODE_USRDROP_REP	57

#define WMM_CODE_USRACCT_REQ	58
#define WMM_CODE_USRACCT_REP	59


//  WMM Attribute Type
#define WMM_ATTR_SECKEY			1
#define WMM_ATTR_BOARD			2
#define WMM_ATTR_ACADDR			3
#define WMM_ATTR_RESCODE		4
#define WMM_ATTR_ECHO			5
#define WMM_ATTR_IDLE			6
#define WMM_ATTR_WIFICFG		7
#define WMM_ATTR_PRTCFG			8
#define WMM_ATTR_FREEIP			9
#define WMM_ATTR_IMGID			10
#define WMM_ATTR_ACCT			11
#define WMM_ATTR_FREEURL		12

#define WMM_ATTR_USERINFO		50
#define WMM_ATTR_USERQUOTA		51
#define WMM_ATTR_USERSTAT		52

//  Result Code
#define WMM_RESULT_CODE_OK		0
#define WMM_RESULT_CODE_FAILED	1
#define WMM_RESULT_CODE_ERROR	2

//  WMM消息
#define WMM_BUFF_SIZE			4096
#define WMM_APID_SIZE			16
#define WMM_APIDSTR_SIZE		36
#define WMM_POS_VER				0
#define WMM_POS_TYPE			1
#define WMM_POS_APID			2
#define WMM_POS_SEQNUM			18
#define WMM_POS_CODE			20
#define WMM_POS_PAYLEN			22
#define WMM_POS_PAYLOAD			24

typedef struct wtpmgrmsg {
	uint8_t version;
	uint8_t type;
	uint16_t seqnum;
	uint16_t code;
	uint16_t payloadlen;
	uint32_t offset;
	uint32_t totlen;
	uint8_t apid[WMM_APID_SIZE];
	uint8_t buff[WMM_BUFF_SIZE];
} WMMSG;


int wmmInit(WMMSG *w, uint8_t ver, uint8_t type, uint8_t *apid, uint16_t seqnum, uint16_t code);

static inline void wmmSetCode(WMMSG *w, uint16_t code)
{
	w->buff[WMM_POS_CODE] = code >> 8;
	w->buff[WMM_POS_CODE + 1] = code;
	w->code = code;
}

static inline void wmmSetPayLen(WMMSG *w, uint16_t paylen)
{
	w->buff[WMM_POS_PAYLEN] = paylen >> 8;
	w->buff[WMM_POS_PAYLEN + 1] = paylen;
	w->payloadlen = paylen;
}

static inline void wmmSetPayZero(WMMSG *w)
{
	wmmSetPayLen(w, 0);
	memset(&(w->buff[WMM_POS_PAYLOAD]), 0, WMM_BUFF_SIZE - WMM_POS_PAYLOAD);
}

static inline void wmmSetOffset(WMMSG *w, uint32_t offset)
{
	w->offset = offset;
}

static inline void wmmReset(WMMSG *w, uint16_t code)
{
	wmmSetCode(w, code);
	wmmSetPayZero(w);
	wmmSetOffset(w, WMM_POS_PAYLOAD);
}

//  明文头
static inline int wmmClearInit(WMMSG *w, uint8_t *apid, uint16_t seqnum, uint16_t code)
{
	return wmmInit(w, WMM_VERSION, WMM_TYPE_CLEAR, apid, seqnum, code);
}
//  协商头
static inline int wmmNegoInit(WMMSG *w, uint8_t *apid, uint16_t seqnum, uint16_t code)
{
	return wmmInit(w, WMM_VERSION, WMM_TYPE_NEGO, apid, seqnum, code);
}
//  密码头
static inline int wmmSTLInit(WMMSG *w, uint8_t *apid, uint16_t seqnum, uint16_t code)
{
	return wmmInit(w, WMM_VERSION, WMM_TYPE_STL, apid, seqnum, code);
}


int wmmPutRawAttr(WMMSG *w, uint16_t type, void *value, uint16_t len);


static inline int wmmPutAttrU8(WMMSG *w, uint16_t type, uint8_t value)
{
	return wmmPutRawAttr(w, type, (void *)&value, sizeof(uint8_t));
}


static inline int wmmPutAttrU16(WMMSG *w, uint16_t type, uint16_t value)
{
	uint16_t _self_ = htons(value);
	return wmmPutRawAttr(w, type, (void *)&_self_, sizeof(uint16_t));
}


static inline int wmmPutAttrU32(WMMSG *w, uint16_t type, uint32_t value)
{
	uint32_t _self_ = htonl(value);
	return wmmPutRawAttr(w, type, (void *)&_self_, sizeof(uint32_t));
}


static inline int wmmPutAttrBytes(WMMSG *w, uint16_t type, uint8_t *value, uint16_t len)
{
	return wmmPutRawAttr(w, type, (void *)value, len);
}


static inline int wmmPutAttrStr(WMMSG *w, uint16_t type, const char *value, uint16_t len)
{
	return wmmPutRawAttr(w, type, (void *)value, len);
}


static inline int wmmPutAttrBuff(WMMSG *w, BUFF *b)
{
	return wmmPutRawAttr(w, b->type, (void *)b->buff, b->offset);
}


int wmmGetAttr(WMMSG *w, BUFF *b);

int wmmPrepRecvd(WMMSG *w);


#endif /* __WTP_MGR_MSG_H__ */
