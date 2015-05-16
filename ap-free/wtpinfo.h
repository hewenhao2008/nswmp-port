/*
 * wtpinfo.h
 *
 *  Created on: 2014年12月24日
 *      Author: LiJie
 */

#ifndef __WTPINFO_H__
#define __WTPINFO_H__

#include "wmpctrl.h"

//  固件主版本号
#define WTP_FIRMWARE_MAJOR_VERSION	1
//  固件子版本号
#define WTP_FIRMWARE_MINOR_VERSION	0


#if 0

#define WTPID_PATH	"/dev/mtdblock3"
#define WTPACADDR_PATH	"/dev/mtdblock4"
#else
#define WTPID_PATH	"/usr/local/natshell/ap/wtpid.conf"
#define WTPACADDR_PATH	"/usr/local/natshell/ap/acaddr.conf"
#endif

#define WTP_ECHO_MAX	1

#define NFHOOK_METHOD_BRIDGE	1
#define NFHOOK_METHOD_ROUTE		2

//  WTP本地状态定义
#define WTP_STATE_INIT	0
#define WTP_STATE_DSCV	1
#define WTP_STATE_SULK	2
#define WTP_STATE_JOIN	3
#define WTP_STATE_IMAGE	4
#define WTP_STATE_RUN	5
#define WTP_STATE_WAIT	6
#define WTP_STATE_RECOV	7


int wtpInfoSetBoardName(const char *v, int len);
const char *wtpInfoGetBoardName(void);

uint32_t wtpInfoSetIdle(uint32_t idle);
uint32_t wtpInfoGetIdle(void);

uint32_t wtpInfoSetEcho(uint32_t echo);
uint32_t wtpInfoGetEcho(void);

uint32_t wtpInfoSetAcct(uint32_t acct);
uint32_t wtpInfoGetAcct(void);

const char *wtpinfoGetACIPStr(void);
const char *wtpinfoSetACIPStr(const char *s);

struct wifi_cfg *wtpInfoGetWIFI(void);

struct prt_cfg *wtpInfoGetPortal(void);

uint16_t wtpInfoGetSEQNUM(void);

const char *wtpidGetstr(void);
const uint8_t *wtpidGetBytes(void);
int wtpidInit(void);

///////////////////////////////////////////////////

int wtpInfoSendApid2Krn(void);
int wtpInfoSendIdle2Krn(void);
int wtpInfoSendPrt2Krn(void);
int wtpInfoSendAcct2Krn(void);

int wtpInfoSendHookOpen2Krn(uint32_t method, const char *dev);
int wtpInfoSendHookClose2Krn(void);
int wtpInfoSendHookPause2Krn(void);
int wtpInfoSendHookRestart2Krn(void);

int wtpWiFiWirteFile(int wlanindex, int encrypt, const char *radio, int channel,
		int ssidsupp, const char *ssid, const char *passwd);

int wtpWiFiOpen(void);

int wtpWiFiClose(void);

int wtpWiFiCloseThe(int wlanindex);


#endif /* __WTPINFO_H__ */
