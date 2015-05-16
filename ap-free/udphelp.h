/*
 * udphelp.h
 *
 *  Created on: 2014年10月3日
 *      Author: LiJie
 */

#ifndef __LIJIE_UDP_HELP_H__
#define __LIJIE_UDP_HELP_H__

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>

/*
 * UDP发送包装函数
 */
static inline int udpSend(int fd, void *buffer, int size,
		struct sockaddr_in *addr) {
	return sendto(fd, buffer, size, 0, (struct sockaddr *) addr,
			sizeof(struct sockaddr_in));
}

/*
 * UDP接收包装函数
 */
static inline int udpRecv(int fd, void *buffer, int size,
		struct sockaddr_in *addr) {
	int addrlen = sizeof(struct sockaddr_in);
	return recvfrom(fd, buffer, size, 0, (struct sockaddr *) addr, &addrlen);
}

/*
 *  UDP关闭连接包装函数
 */
static inline int udpCloseConnect(int fd) {
	return close(fd);
}

/*
 * 产生一个UDP链接套接字
 * 参数 :
 * 		nonblock 	1为非阻塞，0为阻塞模式
 * 		msto		阻塞模式下的超时时间，单位为毫秒
 */
int udpNewConnect(int nonblock, long msto);

/*
 * 产生一个UDP长连接套接字
 * 参数 :
 * 		sip		本地IP地址，可以为空
 * 		sport 	本地端口，可以为空
 * 		dip		目标IP地址，不能为空
 * 		dport 	目标端口
 * 		nonblock 	1为非阻塞，0为阻塞模式
 * 		msto		阻塞模式下的超时时间，单位为毫秒
 * 	返回 :
 * 		>0	套接字描述符
 * 		<0	出错
 */
int udpNewLongConnect(const char *sip, const char *sport, const char *dip,
		const char *dport, int nonblock, unsigned long msto);

/*
 * 产生一个阻塞的UDP套接字
 * 参数 :
 * 		msto	阻塞模式下的超时时间，单位为毫秒
 * 返回 :
 * 		>0	套接字描述符
 * 		<0	出错
 */
static inline int udpNewBlockConnect(long msto) {
	return udpNewConnect(0, msto);
}

/*
 * 产生一个非阻塞的UDP套接字
 * 返回 :
 * 		>0	套接字描述符
 * 		<0	出错
 */
static inline int udpNewNonblockConnect(void) {
	return udpNewConnect(1, 0);
}

/*
 * 产生一个UDP阻塞长连接套接字
 * 参数 :
 * 		sip		本地IP地址，可以为空
 * 		sport 	本地端口，可以为空
 * 		dip		目标IP地址，不能为空
 * 		dport 	目标端口
 * 		msto		阻塞模式下的超时时间，单位为毫秒
 * 返回 :
 * 		>0	套接字描述符
 * 		<0	出错
 */
static inline int udpNewBlockLongConnect(const char *sip, const char *sport,
		const char *dip, const char *dport, unsigned long msto) {
	return udpNewLongConnect(sip, sport, dip, dport, 0, msto);
}

/*
 * 参数 :
 * 		sip		本地IP地址，可以为空
 * 		sport 	本地端口，可以为空
 * 		dip		目标IP地址，不能为空
 * 		dport 	目标端口
 * 返回 :
 * 		>0	套接字描述符
 * 		<0	出错
 */
static inline int udpNewNonblockLongConnect(const char *sip, const char *sport,
		const char *dip, const char *dport) {
	return udpNewLongConnect(sip, sport, dip, dport, 1, 0);
}

/*
 * 产生一个UPD服务
 * 参数 :
 * 		ip		监听的IP地址，可以为空
 * 		port	监听的端口号
 * 		nonblock 	1为非阻塞，0为阻塞模式
 * 		msto		阻塞模式下的超时时间，单位为毫秒
 * 返回 :
 * 		>0	套接字描述符
 * 		<0	出错
 */
int udpNewServer(const char *ip, uint16_t port, int nonblock, long msto);

/*
 * 产生一个阻塞的UPD服务
 * 参数 :
 * 		ip		监听的IP地址，可以为空
 * 		port	监听的端口号
 * 		msto		阻塞模式下的超时时间，单位为毫秒
 * 返回 :
 * 		>0	套接字描述符
 * 		<0	出错
 */
static inline int udpNewBlockServer(const char *ip, uint16_t port, long msto)
{
	return udpNewServer(ip, port, 0, msto);
}

/*
 * 产生一个非阻塞的UPD服务
 * 参数 :
 * 		ip		监听的IP地址，可以为空
 * 		port	监听的端口号
 * 返回 :
 * 		>0	套接字描述符
 * 		<0	出错
 */
static inline int udpNewNonblockServer(const char *ip, uint16_t port)
{
	return udpNewServer(ip, port, 1, 0);
}

/*
 * 其中msec为毫秒级别的超时时间
 * < 0 : error
 * 0 : timeout
 * > 0 : OK
 */
int udpTimeRecv(int fd, void *buff, int size, struct sockaddr_in *from, int msec);

#endif /* __LIJIE_UDP_HELP_H__ */
