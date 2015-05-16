/*
 * ieee80211.h
 *
 *  Created on: 2015年1月15日
 *      Author: LiJie
 */

#ifndef __IEEE80211_H__
#define __IEEE80211_H__

//  IEEE802.11无线类型代码
#define IEEE80211		1
#define IEEE80211A		2
#define IEEE80211B		3
#define IEEE80211C		4
#define IEEE80211D		5
#define IEEE80211E		6
#define IEEE80211F		7
#define IEEE80211G		8
#define IEEE80211H		9
#define IEEE80211I		10
#define IEEE80211J		11
#define IEEE80211K		12
#define IEEE80211L		13
#define IEEE80211M		14
#define IEEE80211N		15
#define IEEE80211O		16
#define IEEE80211P		17
#define IEEE80211Q		18
#define IEEE80211R		19
#define IEEE80211S		20
#define IEEE80211T		21
#define IEEE80211U		22
#define IEEE80211V		23
#define IEEE80211AC		24
#define IEEE80211AD		25
#define IEEE80211AE		26

#define IEEE80211_MAX	IEEE80211AE

static char *ieee80211[] = {
		"error",
		"def"
		"a",
		"b",
		"c",
		"d",
		"e",
		"f",
		"h",
		"g",
		"h",
		"i",
		"j",
		"k",
		"l",
		"m",
		"n",
		"o",
		"p",
		"q",
		"r",
		"s",
		"t",
		"u",
		"v",
		"ac",
		"ad",
		"ae"
};



#endif /* __IEEE80211_H__ */
