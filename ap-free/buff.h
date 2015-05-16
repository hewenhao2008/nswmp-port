/*
 * buffer.h
 *
 *  Created on: 2014年12月16日
 *      Author: LiJie
 */

#ifndef __WMM_BUFF_H__
#define __WMM_BUFF_H__

#include <string.h>
#include <stdint.h>

#define BUFF_SIZE	1024
#define BUFF_INIT	{0, BUFF_SIZE, 0, {0}}

#define BUFF_TYPE_DEF	0
#define BUFF_TYPE_U8	1
#define BUFF_TYPE_U16	2
#define BUFF_TYPE_U32	3
#define BUFF_TYPE_BYTE	4
#define BUFF_TYPE_STR	5

struct buffer {
	uint16_t type;
	uint16_t len;
	int offset;
	char buff[BUFF_SIZE];
};


typedef struct buffer BUFF;

int buffInit(BUFF *b, uint16_t type);

//  BUFFER内容基础填充函数
int buffRawPut(BUFF *b, uint8_t type, const void *value, int len);


static inline int buffPutU8(BUFF *b, uint8_t value)
{
	return buffRawPut(b, BUFF_TYPE_U8, &value, sizeof(uint8_t));
}


static inline int buffPutU16(BUFF *b, uint16_t value)
{
	return buffRawPut(b, BUFF_TYPE_U16, &value, sizeof(uint16_t));
}


static inline int buffPutU32(BUFF *b, uint32_t value)
{
	return buffRawPut(b, BUFF_TYPE_U32, &value, sizeof(uint32_t));
}


static inline int buffPutBytes(BUFF *b, uint8_t *value, int len)
{
	return buffRawPut(b, BUFF_TYPE_BYTE, value, len);
}


static inline int buffPutStr(BUFF *b, const char *value, int len)
{
	return buffRawPut(b, BUFF_TYPE_STR, value, len);
}


static inline int buffPutIPAddr(BUFF *b, uint32_t value)
{
	return buffRawPut(b, BUFF_TYPE_BYTE, &value, sizeof(uint32_t));
}

//  BUFFER内容基础获取函数
int buffRawGet(BUFF *b, uint8_t type, void *value, int len);

static inline int buffGetU8(BUFF *b, uint8_t *value)
{
	return buffRawGet(b, BUFF_TYPE_U8, value, sizeof(uint8_t));
}


static inline int buffGetU16(BUFF *b, uint16_t *value)
{
	return buffRawGet(b, BUFF_TYPE_U16, value, sizeof(uint16_t));
}


static inline int buffGetU32(BUFF *b, uint32_t *value)
{
	return buffRawGet(b, BUFF_TYPE_U32, value, sizeof(uint32_t));
}


static inline int buffGetByte(BUFF *b, uint8_t *value, int len)
{
	return buffRawGet(b, BUFF_TYPE_BYTE, value, len);
}


static inline int buffGetStr(BUFF *b, char *value, int len)
{
	return buffRawGet(b, BUFF_TYPE_STR, value, len);
}

static inline int buffGetIPAddr(BUFF *b, uint32_t *value)
{
	return buffRawGet(b, BUFF_TYPE_BYTE, value, sizeof(uint32_t));
}

#endif /* __WMM_BUFFER_H__ */
