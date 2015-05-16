/*
 * wtpinfo.c
 *
 *  Created on: 2014年12月25日
 *      Author: LiJie
 */


#include <stdio.h>
#include <stdint.h>
#include <netinet/in.h>
#include "log.h"
#include "wtpinfo.h"

static uint32_t S_idle;
static uint32_t S_echo;
static uint32_t S_acct;
static char S_acaddr[20] = { 0 };

static struct wifi_cfg S_wifi;

static char S_wtp_board_name[512];


int wtpInfoSetBoardName(const char *v, int len)
{

	if (!v || len > sizeof(S_wtp_board_name) - 1) {
		return -1;
	}
	memset(S_wtp_board_name, 0, sizeof(S_wtp_board_name));
	strncpy(S_wtp_board_name, v, len);
	return 0;
}

const char *wtpInfoGetBoardName(void)
{
	return S_wtp_board_name;
}


/*
 *
 */
uint32_t wtpInfoSetIdle(uint32_t idle)
{
	S_idle = idle;
	return S_idle;
}

/*
 *
 */
uint32_t wtpInfoGetIdle(void)
{
	return S_idle;
}

/*
 *
 */
uint32_t wtpInfoSetEcho(uint32_t echo)
{
	S_echo = echo;
	return S_echo;
}

/*
 *
 */
uint32_t wtpInfoGetEcho(void)
{
	return S_echo;
}

/*
 *
 */
uint32_t wtpInfoSetAcct(uint32_t acct)
{
	S_acct = acct;
	return S_acct;
}

/*
 *
 */
uint32_t wtpInfoGetAcct(void)
{
	return S_acct;
}

/*
 *
 */
struct wifi_cfg *wtpInfoGetWIFI(void)
{
	return &S_wifi;
}

/*
 *
 */
const char *wtpinfoGetACIPStr(void)
{
	return S_acaddr;
}

/*
 *
 */
const char *wtpinfoSetACIPStr(const char *s)
{
	memset(S_acaddr, 0, sizeof(S_acaddr));
	strncpy(S_acaddr, s, sizeof(S_acaddr) - 1);
	return S_acaddr;
}

/*
 * SEQ NUM
 */
uint16_t wtpInfoGetSEQNUM(void)
{
	static uint16_t seq_num = 65535;

	if (65535 == seq_num) {
		seq_num = random() % 65536;
	} else {
		seq_num++;
	}
	return seq_num;
}
