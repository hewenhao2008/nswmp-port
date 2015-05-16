/*
 * wifi.c
 *
 *  Created on: 2015年1月6日
 *      Author: LiJie
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "log.h"
#include "util.h"
#include "wtpinfo.h"

static char *WiFiPathTemp = "/tmp/hostapd-%d.conf";

static char WiFiTemp[]= "driver=nl80211\n"\
					"logger_syslog=-1\n"\
					"logger_syslog_level=2\n"\
					"logger_stdout=-1"\
					"logger_stdout_level=2\n"\
					"hw_mode=%s\n"\
					"channel=%d\n"\
					"ieee80211n=1"\
					"ht_capab=[LDPC][SHORT-GI-20][SHORT-GI-40][TX-STBC][RX-STBC1][DSSS_CCK-40]\n"\
					"interface=wlan%d\n"\
					"ctrl_interface=/var/run/hostapd-%d\n"\
					"disassoc_low_ack=1\n"\
					"preamble=1\n"\
					"wmm_enabled=1"\
					"ignore_broadcast_ssid=%d\n"\
					"auth_algs=1\n"\
					"wpa=0\n"\
					"ssid=%s\n"\
					"bridge=br-lan\n"\
					"bssid=00:02:03:04:05:06";

static char *WiFiSimplePRTTemp = "ssid=%s\n"\
							"utf8_ssid=1\n"\
							"hw_mode=%s\n"\
							"channel=%d\n"\
							"interface=wlan%d\n"\
							"bridge=br-lan\n"\
							"driver=nl80211\n"\
							"ignore_broadcast_ssid=%d\n";

static char *WiFiSimpleWPATemp = "ssid=%s\n"\
							"utf8_ssid=1\n"\
							"hw_mode=%s\n"\
							"channel=%d\n"\
							"interface=wlan%d\n"\
							"bridge=br-lan\n"\
							"driver=nl80211\n"\
							"ignore_broadcast_ssid=%d\n"\
							"wpa=1\n"\
							"wpa_passphrase=%s\n"\
							"wpa_key_mgmt=WPA-PSK\n"\
							"wpa_pairwise=CCMP";

static char *WiFiSimpleWPA2Temp = "ssid=%s\n"\
							"utf8_ssid=1\n"\
							"hw_mode=%s\n"\
							"channel=%d\n"\
							"interface=wlan%d\n"\
							"bridge=br-lan\n"\
							"driver=nl80211\n"\
							"ignore_broadcast_ssid=%d\n"\
							"wpa=2\n"\
							"wpa_passphrase=%s\n"\
							"wpa_key_mgmt=WPA-PSK\n"\
							"rsn_pairwise=CCMP";

static int S_WiFi_num = 0;

struct wifi_info {
	int wlanindex;
	int ssidsupp;
	int channel;
	char ssid[40];
};

static struct wifi_info wifi[8];

/*
 * 写WiFi配置文件
 */
int wtpWiFiWirteFile(int wlanindex, int encrypt, const char *radio, int channel,
		int ssidsupp, const char *ssid, const char *passwd)
{
	int rc = 0;
	FILE *fp = NULL;
	char path[512] = { 0 };
	char *cfgtmp = NULL;

	if (snprintf(path, sizeof(path), WiFiPathTemp, wlanindex) < 1) {
		logError("snprintf : %s", strerror(errno));
		return -1;
	}
	fp = fopen(path, "w+");
	if (!fp) {
		logError("fopen \"%s\" : %s", path, strerror(errno));
		return -1;
	}
	switch (encrypt) {
		case 0:
			rc = fprintf(fp, WiFiSimplePRTTemp, ssid, radio, channel, wlanindex,
					ssidsupp);
			if (rc < 1) {
				logError("fprintf \"%s\" : %s", path, strerror(errno));
				rc = -1;
			}
			break;
		case 1:
		case 2:
			rc = fprintf(fp, WiFiSimpleWPATemp, ssid, radio, channel, wlanindex,
					ssidsupp, passwd);
			if (rc < 1) {
				logError("fprintf \"%s\" : %s", path, strerror(errno));
				rc = -1;
			}
			break;
		case 3:
		case 4:
			rc = fprintf(fp, WiFiSimpleWPA2Temp, ssid, radio, channel, wlanindex,
					ssidsupp, passwd);
			if (rc < 1) {
				logError("fprintf \"%s\" : %s", path, strerror(errno));
				rc = -1;
			}
			break;
		default:
			logError("Unknown encrypt method %d", encrypt);
			rc = -1;
			break;
	}
	if(rc != -1) {
		rc = 0;
		wifi[S_WiFi_num].wlanindex = wlanindex;
		strncpy(wifi[S_WiFi_num].ssid, ssid, 32);
		wifi[S_WiFi_num].channel = channel;
		wifi[S_WiFi_num].ssidsupp = ssidsupp;
		S_WiFi_num++;
	}
	fclose(fp);
	return rc;
}

/*
 * 打开WiFi
 */
int wtpWiFiOpen(void)
{
	int i = 0;
	int rc = 0;
	char cmd[1024];

	for (i = 0; i < S_WiFi_num; ++i) {
		memset(cmd, 0, sizeof(cmd));
		rc = snprintf(cmd, sizeof(cmd), "/sbin/starthostapd.sh %d &", wifi[i].wlanindex);
		if (rc < 1) {
			logError("snprintf : %s", strerror(errno));
			return -1;
		}
		if (utilDoCMD(cmd)) {
			logError("Open SSID=%s WiFi error", wifi[i].ssid);
			return -1;
		}
		logInfo("Open SSID=%s WiFi OK", wifi[i].ssid);
	}
	return 0;
}

/*
 * 关闭WiFi
 */
int wtpWiFiClose(void)
{
	int i = 0;
	int rc = 0;
	char cmd[1024];

	logWarn("Close WiFi");
	for (i = 0; i < S_WiFi_num; ++i) {
		memset(cmd, 0, sizeof(cmd));
		rc = snprintf(cmd, sizeof(cmd), "/sbin/stophostapd.sh %d &", wifi[i].wlanindex);
		if (rc < 1) {
			logError("snprintf : %s", strerror(errno));
			return -1;
		}
		if (utilDoCMD(cmd)) {
			logError("Close SSID=%s WiFi error", wifi[i].ssid);
			return -1;
		}
		logInfo("Close SSID=%s WiFi OK", wifi[i].ssid);
	}
	return 0;
}

/*
 * 关闭指定WiFi
 * 返回值 :
 * 		0 : OK
 * 		-1 : ERROR
 * 		-2 : NO
 */
int wtpWiFiCloseThe(int wlanindex)
{
	int i = 0;
	int rc = 0;
	char cmd[1024];

	logWarn("Close WiFi");
	for (i = 0; i < S_WiFi_num; ++i) {
		if (wlanindex == wifi[i].wlanindex) {
			memset(cmd, 0, sizeof(cmd));
			rc = snprintf(cmd, sizeof(cmd), "/sbin/stophostapd.sh %d &", wifi[i].wlanindex);
			if (rc < 1) {
				logError("snprintf : %s", strerror(errno));
				return -1;
			}
			if (utilDoCMD(cmd)) {
				logError("Close SSID=%s WiFi error", wifi[i].ssid);
				return -1;
			}
			logInfo("Close SSID=%s WiFi OK", wifi[i].ssid);
			return 0;
		}
	}
	return -2;
}
