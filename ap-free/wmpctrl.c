/*
 * wmmctrl.c
 *
 *  Created on: 2014年12月17日
 *      Author: LiJie
 */


#include <stdio.h>
#include "log.h"
#include "buff.h"
#include "wmpctrl.h"

int wmmPackSecKey(BUFF *b, struct sec_key *v)
{
	if (!b || !v) {
		logError("Args error");
		return -1;
	}
	buffInit(b, WMM_ATTR_SECKEY);
	buffPutU32(b, v->keylen);
	buffPutStr(b, v->rawkey, strlen(v->rawkey));
	return 0;
}

int wmmParseSecKey(BUFF *b, struct sec_key *v)
{
	if (!b || !v) {
		logError("Args error");
		return -1;
	}
	buffGetU32(b, &(v->keylen));
	buffGetStr(b, v->rawkey, b->len - sizeof(uint32_t));
	return ((b->offset == b->len) ? 0 : -1);
}

int wmmPackBoard(BUFF *b, uint16_t major, uint16_t minor, const char *name)
{
	if (!b || !name) {
		logError("Args error");
		return -1;
	}
	buffInit(b, WMM_ATTR_BOARD);
	buffPutU16(b, major);
	buffPutU16(b, minor);
	buffPutStr(b, name, strlen(name));
	return 0;
}

int wmmParseBoard(BUFF *b, struct board *v)
{
	if (!b || !v) {
		logError("Args error");
		return -1;
	}
	buffGetU16(b, &(v->firmmajor));
	buffGetU16(b, &(v->firmminor));
	buffGetStr(b, v->hardware, b->len - sizeof(uint32_t));
	return ((b->offset == b->len) ? 0 : -1);
}

int wmmPackResCode(BUFF *b, uint32_t code)
{
	if (!b) {
		logError("Args error");
		return -1;
	}
	buffInit(b, WMM_ATTR_RESCODE);
	buffPutU32(b, code);
	return 0;
}

int wmmParseResCode(BUFF *b, uint32_t *v)
{
	if (!b || !v) {
		logError("Args error");
		return -1;
	}
	buffGetU32(b, v);
	return ((b->offset == b->len) ? 0 : -1);
}

int wmmPackEcho(BUFF *b, uint32_t v)
{
	if (!b) {
		logError("Args error");
		return -1;
	}
	buffInit(b, WMM_ATTR_ECHO);
	buffPutU32(b, v);
	return 0;
}

int wmmParseEcho(BUFF *b, uint32_t *v)
{
	if (!b || !v) {
		logError("Args error");
		return -1;
	}
	buffGetU32(b, v);
	return ((b->offset == b->len) ? 0 : -1);
}

int wmmPackIdle(BUFF *b, uint32_t v)
{
	if (!b) {
		logError("Args error");
		return -1;
	}
	buffInit(b, WMM_ATTR_IDLE);
	buffPutU32(b, v);
	return 0;
}

int wmmParseIdle(BUFF *b, uint32_t *v)
{
	if (!b || !v) {
		logError("Args error");
		return -1;
	}
	buffGetU32(b, v);
	return ((b->offset == b->len) ? 0 : -1);
}


int wmmParseWIFI(BUFF *b, struct wifi_cfg *v)
{
	if (!b || !v) {
		logError("Args error");
		return -1;
	}
	buffGetU8(b, &(v->radiotype));
	buffGetU8(b, &(v->channel));
	buffGetU8(b, &(v->encrypt));
	buffGetU8(b, &(v->arithmetic));
	buffGetU8(b, &(v->keylen));
	buffGetU8(b, &(v->acctenable));
	buffGetU16(b, &(v->power));
	buffGetU16(b, &(v->wlanindex));
	buffGetU8(b, &(v->ssidsupp));
	buffGetU8(b, &(v->ssidlen));
	buffGetStr(b, v->ssid, v->ssidlen);
	buffGetStr(b, v->key, v->keylen);

	return ((b->offset == b->len) ? 0 : -1);
}
