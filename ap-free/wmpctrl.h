/*
 * wmmctrl.h
 *
 *  Created on: 2014年12月17日
 *      Author: LiJie
 */

#ifndef __WMM_CTRL_H__
#define __WMM_CTRL_H__

#include <stdint.h>
#include "buff.h"
#include "wmp.h"

//
struct sec_key {
	uint32_t keylen;
	char rawkey[128];
};

struct board {
	uint16_t firmmajor;
	uint16_t firmminor;
	char hardware[128];
};

struct img_ident {
	uint32_t filesize;
	char filehash[33];
	char name[256];
};

struct free_url {
	uint16_t urllen;
	char *url[256];
};

struct wifi_cfg {
	uint8_t radiotype;
	uint8_t channel;
	uint16_t wlanindex;
	uint8_t ssidsupp;
	uint8_t ssidlen;
	uint16_t power;
	uint8_t encrypt;
	uint8_t arithmetic;
	uint8_t keylen;
	uint8_t acctenable;
	char ssid[40];
	char key[40];
};

int wmmPackSecKey(BUFF *b, struct sec_key *k);
int wmmParseSecKey(BUFF *b, struct sec_key *v);

int wmmPackBoard(BUFF *b, uint16_t major, uint16_t minor, const char *name);
int wmmParseBoard(BUFF *b, struct board *v);

int wmmPackResCode(BUFF *b, uint32_t code);
int wmmParseResCode(BUFF *b, uint32_t *code);

int wmmPackEcho(BUFF *b, uint32_t echo);
int wmmParseEcho(BUFF *b, uint32_t *echo);

int wmmPackIdle(BUFF *b, uint32_t idle);
int wmmParseIdle(BUFF *b, uint32_t *idle);

int wmmParseWIFI(BUFF *b, struct wifi_cfg *v);

#endif /* __WMM_CTRL_H__ */
