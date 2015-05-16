/*
 * udphelp.c
 *
 *  Created on: 2014年10月3日
 *      Author: LiJie
 */

#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <netdb.h>  //  for getaddrinfo
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/select.h>
#include "log.h"
#include "udphelp.h"

/*
 * 设置fd为非阻塞模式
 */
static int udpSetNonblock(int fd)
{
	int flags = 0;
	if (fd < 0) {
		logError("argument is error");
		return -1;
	}
	if ((flags = fcntl(fd, F_GETFL, 0)) == -1) {
		logError("fcntl : %s", strerror(errno));
		return -1;
	} else if (fcntl(fd, F_SETFL, flags | O_NONBLOCK | O_ASYNC) != 0) {
		logError("fcntl : %s", strerror(errno));
		return -1;
	}
	return 0;
}

/*
 * 产生一个UDP套接字
 * 参数 :
 * 		nonblock 	1为非阻塞，0为阻塞模式
 * 		msto		阻塞模式下的超时时间，单位为毫秒
 */
int udpNewConnect(int nonblock, long msto)
{
	int fd = -1;
	struct sockaddr_in sin;

	fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (fd < 0) {
		logError("socket : %s", strerror(errno));
		return -1;
	}
	//  判断是否阻塞
	if (1 == nonblock) {
		if (udpSetNonblock(fd) < 0) {
			close(fd);
			return -1;
		}
	} else {
		if (msto < 1) {
			logError("timeout value error");
			close(fd);
			return -1;
		}
		struct timeval tv;
		tv.tv_sec = msto / 1000;
		tv.tv_usec = (msto % 1000) * 1000;
		if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
			logError("setsockopt : %s", strerror(errno));
			close(fd);
			return -1;
		}
	}
	memset(&sin, 0, sizeof sin);
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons(0);
	if (bind(fd, (const struct sockaddr *)&(sin), sizeof(sin)) == -1) {
		logError("bind : %s", strerror(errno));
		close(fd);
		return -1;
	}
	return fd;
}

/*
 * 产生一个UDP长连接套接字
 * 参数 :
 * 		sip		本地IP地址，可以为空
 * 		sport 	本地端口，可以为空
 * 		dip		目标IP地址，不能为空
 * 		dport 	目标端口
 * 		nonblock 	1为非阻塞，0为阻塞模式
 * 		msto		阻塞模式下的超时时间，单位为毫秒
 */
int udpNewLongConnect(const char *sip,
					  const char *sport,
					  const char *dip,
					  const char *dport,
					  int nonblock,
					  unsigned long msto)
{
	int fd = -1;
	int yes = 1;
	struct sockaddr_in src;
	struct sockaddr_in dst;
	struct addrinfo hints;
	struct addrinfo *res = NULL;
	char myport[8] = { 0 };

	if (!dip || !dport) {
		logError("arguments is error");
		return -1;
	}
	memset(&dst, 0, sizeof(dst));
	dst.sin_family = AF_INET;
	dst.sin_addr.s_addr = inet_addr(dip);
	dst.sin_port = htons(atol(dport));
	if (dst.sin_port > 0xffff) {
		logError("port \"%s\" is invalid", dport);
		return -1;
	}
	fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (fd < 0) {
		logError("socket : %s", strerror(errno));
		return -1;
	}
	//  判断是否阻塞
	if (1 == nonblock) {
		if (udpSetNonblock(fd) < 0) {
			close(fd);
			return -1;
		}
	} else {
		if (msto < 1) {
			logError("timeout value error");
			close(fd);
			return -1;
		}
		struct timeval tv;
		tv.tv_sec = msto / 1000;
		tv.tv_usec = (msto % 1000) * 1000;
		if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
			logError("setsockopt : %s", strerror(errno));
			close(fd);
			return -1;
		}
	}
	//  绑定本地地址与端口
	if (!sip && !sport) {
		memset(&hints, 0, sizeof (struct addrinfo));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_flags = AI_PASSIVE;
		//  绑定本地端口
		getaddrinfo(NULL, myport, &hints, &res);
		if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes))) {
			logError("setsockopt : %s", strerror(errno));
			close(fd);
			return -1;
		}
		if (bind(fd, res->ai_addr, res->ai_addrlen) < 0) {
			logError("bind : %s", strerror(errno));
			close(fd);
			return -1;
		}
	} else {
		memset(&src, 0, sizeof(src));
		src.sin_family = AF_INET;
		if (!sip) {
			src.sin_addr.s_addr = INADDR_ANY;
		} else {
			src.sin_addr.s_addr = inet_addr(sip);
		}
		src.sin_port = htons(atol(sport));
		if (src.sin_port > 0xffff) {
			logError("remote port \"%s\" is invalid", sport);
			close(fd);
			return -1;
		}
		if (bind(fd, (struct sockaddr *)&src, sizeof(src)) < 0) {
			logError("bind : %s", strerror(errno));
			close(fd);
			return -1;
		}
	}
#if 0
	//  绑定目标地址与端口
	if (connect(fd, (struct sockaddr *)&dst, sizeof(dst))) {
		logError("connect : %s", strerror(errno));
		close(fd);
		return -1;
	}
#endif
	return fd;
}

/*
 * 产生一个UPD服务
 */
int udpNewServer(const char *ip,
		  	     uint16_t port,
		  	     int nonblock,
		  	     long msto)
{
	int fd = -1;
	struct sockaddr_in addr;

	if(port < 1){
		logError("Args error");
		return -1;
	}
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	if (ip) {
		addr.sin_addr.s_addr = inet_addr(ip);
	} else {
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
	}
	addr.sin_port = htons(port);

	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if(fd < 0){
		logError("socket : %s", strerror(errno));
		return (-1);
	}
	if (1 == nonblock) {
		if (udpSetNonblock(fd) < 0) {
			logError("Set socket no block error");
			close(fd);
			return -1;
		}
	} else {
		if (msto < 1) {
			logError("timeout value error");
			close(fd);
			return -1;
		}
		struct timeval tv;
		tv.tv_sec = msto / 1000;
		tv.tv_usec = (msto % 1000) * 1000;
		if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
			logError("setsockopt : %s", strerror(errno));
			close(fd);
			return -1;
		}
	}
	if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0){
		logError("bind : %s", strerror(errno));
		close(fd);
		return (-1);
	}
	return fd;
}

/*
 * 其中msec为毫秒级别的超时时间
 * < 0 : error
 * 0 : timeout
 * > 0 : OK
 */
int udpTimeRecv(int fd, void *buff, int size, struct sockaddr_in *from, int msec)
{
	int rc = 0;
	struct timeval tv;
	fd_set fset;
	int addrlen = sizeof(struct sockaddr_in);

	if (fd < 1 || !buff || !from) {
		logError("Arguments is error");
		return -1;
	}
	tv.tv_sec = msec / 1000;
	tv.tv_usec = ((msec % 1000) * 1000);
	FD_ZERO(&fset);
	FD_SET(fd, &fset);
	rc = select(fd + 1, &fset, NULL, NULL, &tv);
	if (rc > 0) {
		if (FD_ISSET(fd, &fset)) {
			rc = recvfrom(fd, buff, size, 0, (struct sockaddr *)from, &addrlen);
			if (rc > 0) {
				return rc;
			} else {
				logError("recvfrom : %s", strerror(errno));
				return -1;
			}
		} else {
			logError("select return not is FD %d", fd);
			return -1;
		}
	} else if (0 == rc) {
		logDebug("Select Time Expired");
		return 0;
	} else {
		if (errno == EINTR) {
			logError("Select Interrupted by signal");
			return -1;
		}
	}
	return -1;
}
