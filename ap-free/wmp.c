/*
 * wmm.c
 *
 *  Created on: 2014年12月16日
 *      Author: LiJie
 */

#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include "log.h"
#include "wmp.h"

/*
 *
 */
int wmmInit(WMMSG *w, uint8_t ver, uint8_t type, uint8_t *apid, uint16_t seqnum, uint16_t code)
{
	if (!w || !apid) {
		logError("Args error");
		return -1;
	}
	memset(w, 0, sizeof(WMMSG));
	w->buff[WMM_POS_VER] = ver;
	w->buff[WMM_POS_TYPE] = type;
	memcpy(&(w->buff[WMM_POS_APID]), apid, WMM_APID_SIZE);
	w->buff[WMM_POS_SEQNUM] = seqnum >> 8;
	w->buff[WMM_POS_SEQNUM + 1] = seqnum;
	w->buff[WMM_POS_CODE] = code >> 8;
	w->buff[WMM_POS_CODE + 1] = code;
	w->offset = WMM_POS_PAYLOAD;
	return 0;
}

/*
 *
 */
int wmmPutRawAttr(WMMSG *w, uint16_t type, void *value, uint16_t len)
{
	if (len > BUFF_SIZE) {
		logError("Attribute is too long");
		return -1;
	}
	if (w->offset + 4 + len > WMM_BUFF_SIZE) {
		logError("Maximum message length exceeded");
		return -1;
	}
	w->buff[w->offset++] = type >> 8;
	w->buff[w->offset++] = type;
	w->buff[w->offset++] = len >> 8;
	w->buff[w->offset++] = len;
	memcpy(&w->buff[w->offset], value, len);
	w->offset += len;
	w->payloadlen += (len + 4);
	w->buff[WMM_POS_PAYLEN] = w->payloadlen >> 8;
	w->buff[WMM_POS_PAYLEN + 1] = w->payloadlen;
	return 0;
}


/*
 *  获取属性，成功返回1，没有属性返回0，失败则返回-1.
 */
int wmmGetAttr(WMMSG *w, BUFF *b)
{
	if (!w || !b) {
		logError("Args error");
		return -1;
	}

	if (w->offset == w->totlen || w->offset > w->totlen) {
		return 0;
	}
	//  检测是否循环到边界
	if (w->offset + 4 > w->totlen) {
		logError("Malformed attribute in response");
		return -1;
	}
	//  type
	b->type = w->buff[w->offset] << 8 | w->buff[w->offset + 1];
	w->offset += 2;
	//  length
	b->len = w->buff[w->offset] << 8 | w->buff[w->offset + 1];
	w->offset += 2;
	//  value
	if (w->offset + b->len > w->totlen) {
		logError("Malformed attribute in response");
		return -1;
	}
	//  value
	if (b->len > BUFF_SIZE) {
		logError("Attribute value is too long %d > %d", b->len, BUFF_SIZE);
		return -1;
	}
	memcpy(b->buff, &(w->buff[w->offset]), b->len);
	w->offset += b->len;
	b->offset = 0;
	return 1;
}

/*
 * 该函数处理接收到的WMM消息头
 */
int wmmPrepRecvd(WMMSG *w)
{
	if (!w) {
		logError("Args error");
		return -1;
	}
	if (w->totlen < WMM_POS_PAYLOAD) {
		logError("Message is too short");
		return -1;
	}
	//  有效消息长度
	w->payloadlen = w->buff[WMM_POS_PAYLEN] << 8 | w->buff[WMM_POS_PAYLEN + 1];
	if (w->totlen < w->payloadlen) {
		logError("The actual length is less than the given length");
		return -1;
	}
	w->offset = WMM_POS_PAYLOAD;
	//  版本
	w->version = w->buff[WMM_POS_VER];
	//  加密类型
	w->type = w->buff[WMM_POS_TYPE];
	//  AP ID
	memcpy(w->apid, &(w->buff[WMM_POS_APID]), WMM_APID_SIZE);
	//  需要解密
	//  随机码
	w->seqnum = w->buff[WMM_POS_SEQNUM] << 8 | w->buff[WMM_POS_SEQNUM + 1];
	//  消息code
	w->code = w->buff[WMM_POS_CODE] << 8 | w->buff[WMM_POS_CODE + 1];

	return 0;
}
