/*
 * By LiJie
 */
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "log.h"

//  控制台输出颜色定义
#define CLR_NONE          "\033[m"
#define CLR_RED           "\033[0;32;31m"
#define CLR_LIGHT_RED     "\033[1;31m"
#define CLR_GREEN         "\033[0;32;32m"
#define CLR_LIGHT_GREEN   "\033[1;32m"
#define CLR_BLUE          "\033[0;32;34m"
#define CLR_LIGHT_BLUE    "\033[1;34m"
#define CLR_GRAY          "\033[0;37m"
#define CLR_DARY_GRAY     "\033[1;30m"
#define CLR_CYAN          "\033[0;36m"
#define CLR_LIGHT_CYAN    "\033[1;36m"
#define CLR_PURPLE        "\033[0;35m"
#define CLR_LIGHT_PURPLE  "\033[1;35m"
#define CLR_BROWN         "\033[0;33m"
#define CLR_YELLOW        "\033[0;33m"
#define CLR_LIGHT_YELLOW  "\033[1;33m"
#define CLR_WHITE         "\033[1;37m"


//   对应颜色选择
static char *__log_colour_array[] = {
    CLR_WHITE,
    CLR_WHITE,
    CLR_WHITE,
    CLR_RED,
    CLR_YELLOW,
    CLR_GREEN,
    CLR_BLUE,
    CLR_GREEN
};

//   对应输出级别选择 
static char *__log_level_string[] = {
    "EMESG",
    "ALERT",
    "CRIT",
    "Error",
    "Warn",
    "Notice",
    "Info",
    "Debug"
};

//  日志级别
volatile int __log_level = LOG_LEVEL_WARN;

//  多线程的互斥锁
static pthread_mutex_t S_log_wr_mutex = PTHREAD_MUTEX_INITIALIZER;

//  日志信息输出文件
static FILE *S_log_out_file = NULL;

//  标准文件输出标志
static int S_log_out_std = 1;



/*
 * 初始化日志输出、级别设置(请注意该路径要具有可写权限)
 * 参数 ：
 * 		file  输出文件路径及名字
 * 		level 输出级别
 */
int logOpen(const char *file, int level)
{
	//  设置日志输出文件
	if (!file) {
		S_log_out_file = stderr;
	} else {
		if (!strcmp(file, "stdout")) {
			S_log_out_file = stdout;
		} else if (!strcmp(file, "stderr")) {
			S_log_out_file = stderr;
		} else {
			FILE *f = fopen(file, "a");
			if (!f) {
				S_log_out_file = stderr;
				S_log_out_std = 1;
				return -1;
			}
			S_log_out_std = 0;
			S_log_out_file = f;
		}
	}
	//  日志输出级别
	if (level > -1) {
		__log_level = level > LOG_LEVEL_DEBUG ? LOG_LEVEL_DEBUG : level;
	} else {
		if (!S_log_out_std) {
			fclose(S_log_out_file);
		}
		S_log_out_file = stderr;
		S_log_out_std = 1;
		return -1;
	}
	return 0;
}

/*
 * 关闭日志输出
 */
void logClose(void)
{
	pthread_mutex_lock(&S_log_wr_mutex);
	if (!S_log_out_std) {
		fclose(S_log_out_file);
	}
	S_log_out_file = stderr;
	S_log_out_std = 1;
	pthread_mutex_unlock(&S_log_wr_mutex);
}

/*
 * 该函数不带颜色输出，适合程序发布时记录运行日志
 */
void __log_base_printf(int level, const char *format, ...)
{
	time_t t;
	time(&t);
	struct tm tm;
	localtime_r((const time_t*) &t, &tm);
	va_list vlist;
	if (S_log_out_file) {
		pthread_mutex_lock(&S_log_wr_mutex);
		fprintf(S_log_out_file, "%04d-%02d-%02d %02d:%02d:%02d %s : ",
				tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
				tm.tm_hour, tm.tm_min, tm.tm_sec, __log_level_string[level]);
		va_start(vlist, format);
		vfprintf(S_log_out_file, format, vlist);
		va_end(vlist);
		fprintf(S_log_out_file, "\n");
		fflush(S_log_out_file);
		pthread_mutex_unlock(&S_log_wr_mutex);
	}
}

/*
 * 该函数带有颜色的指令输出，适合控制台Debug调试时使用
 */
void __log_base_colour_printf(int level, const char *filename, int line, const char *function,const char *format, ...)
{
	time_t t;
	time(&t);
	struct tm tm;
	localtime_r((const time_t*) &t, &tm);
	va_list vlist;
	if (S_log_out_file) {
		pthread_mutex_lock(&S_log_wr_mutex);
		fprintf(S_log_out_file, "%s[%04d-%02d-%02d %02d:%02d:%02d] %s %s:%d %s: ",
				__log_colour_array[level], tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
				tm.tm_hour, tm.tm_min, tm.tm_sec, __log_level_string[level],
				filename, line, function);
		va_start(vlist, format);
		vfprintf(S_log_out_file, format, vlist);
		va_end(vlist);
		fprintf(S_log_out_file, CLR_NONE"\n");
		fflush(S_log_out_file);
		pthread_mutex_unlock(&S_log_wr_mutex);
	}
}
