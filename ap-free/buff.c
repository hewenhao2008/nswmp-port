/*
 * buffer.c
 *
 *  Created on: 2014年12月17日
 *      Author: LiJie
 */


#include <stdio.h>
#include <string.h>
#include "log.h"
#include "buff.h"

int buffInit(BUFF *b, uint16_t type)
{
	if (!b) {
		logError("Arg error");
	}
	memset(b, 0, sizeof(BUFF));
	b->type = type;
	b->len = BUFF_SIZE;
	return 0;
}

/*
 *
 */
int buffRawPut(BUFF *b, uint8_t type, const void *value, int len)
{
	if (!b || !value || len < 1) {
		logError("Args error");
		return -1;
	}
	if (b->offset + len > BUFF_SIZE) {
		logError("Maximum message length exceeded");
		return -1;
	}
	if (BUFF_TYPE_U16 == type) {
		uint16_t tmp = *(uint16_t *)value;
		uint16_t tmp16 = htons(tmp);
		if (!memcpy(&(b->buff[b->offset]), &tmp16, sizeof(uint16_t))) {
			logError("memcpy : %s", strerror(errno));
			return -1;
		}
	} else if (BUFF_TYPE_U32 == type) {
		uint32_t tmp = *(uint32_t *)value;
		uint32_t tmp32 = htonl(tmp);
		if (!memcpy(&(b->buff[b->offset]), &tmp32, sizeof(uint32_t))) {
			logError("memcpy : %s", strerror(errno));
			return -1;
		}
	} else {
		if (!memcpy(&(b->buff[b->offset]), value, len)) {
			logError("memcpy : %s", strerror(errno));
			return -1;
		}
	}
	b->offset += len;
	b->len = b->offset;
	return 0;
}

/*
 *
 */
int buffRawGet(BUFF *b, uint8_t type, void *value, int len)
{
	if (!b || !value || len < 0) {
		logError("Args error");
		return -1;
	}
	if (0 == len) {
		return 0;
	}
	if (b->offset + len > b->len) {
		logError("Maximum message length exceeded");
		return -1;
	}

	if (BUFF_TYPE_U16 == type) {
		uint16_t tmp = 0;
		if (!memcpy(&tmp, &(b->buff[b->offset]), sizeof(uint16_t))) {
			logError("memcpy : %s", strerror(errno));
			return -1;
		}
		tmp = ntohs(tmp);
		memcpy(value, &tmp, sizeof(uint16_t));
	} else if (BUFF_TYPE_U32 == type) {
		uint32_t tmp = 0;
		if (!memcpy(&tmp, &(b->buff[b->offset]), sizeof(uint32_t))) {
			logError("memcpy : %s", strerror(errno));
			return -1;
		}
		tmp = ntohl(tmp);
		memcpy(value, &tmp, sizeof(uint32_t));
	} else {
		if (!memcpy(value, &(b->buff[b->offset]), len)) {
			logError("memcpy : %s", strerror(errno));
			return -1;
		}
	}
	b->offset += len;
	return 0;
}
