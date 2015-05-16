/*
 * sulk.c
 *
 *  Created on: 2014年12月25日
 *      Author: LiJie
 */


#include <stdio.h>
#include <netinet/in.h>
#include "log.h"
#include "wmpctrl.h"
#include "udphelp.h"
#include "wtpinfo.h"
#include "util.h"

extern int G_wmpsock;

//  AP静默时间
static int S_sulktime = 10;

static int wmpSulkRecvAC(int fd);

/*
 *
 */
void wmpSulk(int *state)
{
	int rc = 0;
	fd_set fset;
	struct timeval tv;
	int max_fd = G_wmpsock;


	tv.tv_sec = S_sulktime;
	tv.tv_usec = 0;

	do {
		FD_ZERO(&fset);
		FD_SET(G_wmpsock, &fset);
		rc = select(max_fd + 1, &fset, NULL, NULL, &tv);
		if (rc > 0) {
			//  AC发来消息
			if (FD_ISSET(G_wmpsock, &fset)) {
				wmpSulkRecvAC(G_wmpsock);
			}
		}
	} while (rc > 0);
	//  超时，发送ECHO
	if (0 == rc) {
		*state = WTP_STATE_DSCV;
	} else {
		//  出错,重启AP
		utilDoCMD("reboot");
	}
}

/*
 *
 */
static int wmpSulkRecvAC(int fd)
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
	return 0;
}
