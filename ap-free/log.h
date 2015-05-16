#ifndef __LIJIE_LOG_H__
#define __LIJIE_LOG_H__

#include <errno.h>

#define LOG_LEVEL_EMESG		0	//   致命级
#define LOG_LEVEL_ALERT		1	//   警戒级
#define LOG_LEVEL_CRIT		2	//   临界级
#define LOG_LEVEL_ERROR		3	//   错误级
#define LOG_LEVEL_WARN		4	//   告警级
#define LOG_LEVEL_NOTICE	5	//   注意级
#define LOG_LEVEL_INFO		6	//   通知级
#define LOG_LEVEL_DEBUG		7	//   调试级


extern volatile int __log_level;

void __log_base_printf(int level, const char *format, ...);
void __log_base_colour_printf(int level, const char *filename, int line, const char *function,const char *format, ...);


#define LOG_LEVEL(level) LOG_LEVEL_##level


/*
 * 编译时请注意定义宏
 */
#ifdef __DEBUG__
#define LOG_BASE_PRINTF(level, format, args...) __log_base_colour_printf(LOG_LEVEL(level),__FILE__, __LINE__, __FUNCTION__, format, ##args)
#else
#define LOG_BASE_PRINTF(level, format, args...) __log_base_printf(LOG_LEVEL(level), format, ##args)
#endif

#define logDebug(format, args...)	((__log_level > LOG_LEVEL_INFO) ? LOG_BASE_PRINTF(DEBUG, format, ##args) : (void)0)

#define logInfo(format, args...)	((__log_level > LOG_LEVEL_NOTICE) ? LOG_BASE_PRINTF(INFO, format, ##args) : (void)0)

#define logWarn(format, args...)	((__log_level > LOG_LEVEL_ERROR) ? LOG_BASE_PRINTF(WARN,  format, ##args) : (void)0)

#define logError(format, args...)	((__log_level > LOG_LEVEL_CRIT) ? LOG_BASE_PRINTF(ERROR, format, ##args) : (void)0)

//  初始化日志输出、级别设置
int logOpen(const char *file, int level);

//  关闭日志输出
void logClose(void);

#endif // __LIJIE_LOG_H__
