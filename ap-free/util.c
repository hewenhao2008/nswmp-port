/*
 * util.c
 *
 *  Created on: 2015年1月6日
 *      Author: LiJie
 */

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include "log.h"
#include "util.h"


/*
 * 设置fd为非阻塞模式
 */
int utilSetNonblock(int fd)
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



int utilDoCMD(const char *cmd)
{
	int rc = 0;
	FILE *fp = NULL;

	if (!cmd) {
		logError("Arg error");
		return -1;
	}

	fp = popen(cmd, "r");
	if (!fp) {
		logError("popen : %s", strerror(errno));
		return -1;
	}
	rc = pclose(fp);

	rc = WEXITSTATUS(rc);
	if (0 == rc) {
		return 0;
	} else {
		logError("Do \"%s\" result failed, code %d", cmd, rc);
		return -1;
	}
	return 0;
}

/*
 * 转换字符串MAC地址
 */
int utilByteMAC(const char *s, uint8_t *m)
{
	int rc = 0;
	char p = 0;
	static char *template = "%02X%[:-]%02X%[:-]%02X%[:-]%02X%[:-]%02X%[:-]%02X";


	if (!s || !m) {
		logError("Args error");
		return -1;
	}
	if (strlen(s) != 17) {
		logError("String MAC format is error");
		return -1;
	}

	rc = sscanf(s, template,
			&(m[0]), &p,
			&(m[1]), &p,
			&(m[2]), &p,
			&(m[3]), &p,
			&(m[4]), &p,
			&(m[5]));

	if (rc != 11) {
		logError("sscanf : %s", strerror(errno));
		rc = -1;
	}
	return 0;
}


///////////////////////////////////////////////////////////////////////////////////////

#if 0

int main(int argc, char **argv)
{
	char *s1 = "00-01-02-03-04-05";
	char *s2 = "00:01:02:03:04:05";
	char *s3 = "AA-01-02-03-04-05";
	char *s4 = "00-01-02-aa-04-05";

	uint8_t mac[6] = {0};

	logOpen("stdout", 7);

	if (utilByteMAC(s1, mac)) {
		logError("MAC parse error");
		return -1;
	}
	logDebug("%02X:%02X:%02X:%02X:%02X:%02X", mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
	memset(mac, 0, sizeof(mac));

	if (utilByteMAC(s2, mac)) {
		logError("MAC parse error");
		return -1;
	}
	logDebug("%02X:%02X:%02X:%02X:%02X:%02X", mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
	memset(mac, 0, sizeof(mac));

	if (utilByteMAC(s3, mac)) {
		logError("MAC parse error");
		return -1;
	}
	logDebug("%02X:%02X:%02X:%02X:%02X:%02X", mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
	memset(mac, 0, sizeof(mac));

	if (utilByteMAC(s4, mac)) {
		logError("MAC parse error");
		return -1;
	}
	logDebug("%02X:%02X:%02X:%02X:%02X:%02X", mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
	memset(mac, 0, sizeof(mac));
	return 0;
}

#endif
