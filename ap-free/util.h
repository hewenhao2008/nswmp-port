/*
 * util.h
 *
 *  Created on: 2015年1月6日
 *      Author: LiJie
 */

#ifndef __WTP_UTIL_H__
#define __WTP_UTIL_H__

#include <stdint.h>

int utilSetNonblock(int fd);

int utilDoCMD(const char *cmd);

int utilByteMAC(const char *s, uint8_t *m);

#endif /* __WTP_UTIL_H__ */
