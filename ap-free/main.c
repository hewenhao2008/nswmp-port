/*
 * main.c
 *
 *  Created on: 2014年12月24日
 *      Author: LiJie
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <getopt.h>
#include <pthread.h>
#include <netinet/in.h>
#include "log.h"
#include "wmpctrl.h"
#include "wtpinfo.h"

static int S_log_level = LOG_LEVEL_WARN;
static int S_debug_flag = 0;

//  与AC通信的UDP套接字
int G_wmpsock = -1;
//  AC通信地址
struct sockaddr_in G_wmpaddr;
//  AP状态
static int S_state = WTP_STATE_INIT;


void mainExit(void)
{
	//  关闭WIFI
	wtpWiFiClose();
	logClose();
	if (G_wmpsock > 0) {
		close(G_wmpsock);
	}
}


void exitSignalHandle(int sig)
{
	logWarn("signal %d, the progress exit", sig);
	mainExit();
	exit(1);
}

void signalsInit(void)
{
	signal(SIGINT, exitSignalHandle);
	signal(SIGKILL, exitSignalHandle);
}

/*
 * usage
 */
static void usage(void)
{
	printf("Usage:\n-h	--help \n");
	printf("-d	--debug \n");
	exit(-1);
}

const char *short_options = "hd";
const struct option long_options[] = {
		{"help",	0,	NULL,	'h'},
		{"debug",	0,	NULL,   'd'},
		{ NULL,	    0,	NULL,	 0}
};

/*
 * 解析启动参数
 */
static void parseArgs(int argc, char **argv)
{
	int rc = 0;
	while((rc = getopt_long(argc, argv, short_options, long_options, NULL)) != -1) {
		switch(rc){
		case 'h':
			usage();
			break;
		case 'd':
			S_log_level = LOG_LEVEL_DEBUG;
			S_debug_flag = 1;
			break;
		default :
			break;
		}
	}
}

extern void wmpInit(int *state);
extern void wmpDscv(int *state);
extern void wmpSulk(int *state);
extern void wmpJoin(int *state);
extern void wmpRun(int *state);
extern void wmpWait(int *state);
extern void wmpRecov(int *state);

int main(int argc, char **argv)
{
	parseArgs(argc, argv);

	//  设置日志,后台运行
	if (0 == S_debug_flag) {
		logOpen("/tmp/ap.log", LOG_LEVEL_WARN);
		daemon(1, 1);
	} else {
		logOpen("stdout", LOG_LEVEL_DEBUG);
	}
	//  注册信号函数
	signalsInit();

	for ( ; ; ) {
		switch (S_state) {
			case WTP_STATE_INIT :
				//  解析配置，包括APID、AC地址
				logDebug("==========INIT STATE==========");
				wmpInit(&S_state);break;
			case WTP_STATE_DSCV :
				logDebug("==========DSCV STATE==========");
				wmpDscv(&S_state);break;
			case WTP_STATE_SULK :
				logDebug("==========SULK STATE==========");
				wmpSulk(&S_state);break;
			case WTP_STATE_JOIN :
				logDebug("==========JOIN STATE==========");
				wmpJoin(&S_state);break;
			case WTP_STATE_IMAGE :
				logDebug("==========IMAGE STATE==========");
				break;
			case WTP_STATE_RUN :
				logDebug("==========RUN STATE==========");
				wmpRun(&S_state);break;
			case WTP_STATE_WAIT :
				logDebug("==========WAIT STATE==========");
				wmpWait(&S_state);break;
			case WTP_STATE_RECOV :
				logDebug("==========RECOV STATE==========");
				wmpRecov(&S_state);
				break;
			default :
				break;
		}
	}
	return 0;
}
