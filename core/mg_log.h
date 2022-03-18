/*================================================================
*
*  文件名称：mg_log.h
*  创 建 者: mongia
*  创建日期：2021年04月01日
*
================================================================*/
#ifndef __MG_LOG_INCLUDE_H__
#define __MG_LOG_INCLUDE_H__

#include "mg_core.h"
#include "mg_time.h"

#define NONE            "\033[m"
#define RED             "\033[0;32;31m"
#define LIGHT_RED       "\033[1;31m"
#define GREEN           "\033[0;32;32m"
#define LIGHT_GREEN     "\033[1;32m"
#define BLUE            "\033[0;32;34m"
#define LIGHT_BLUE      "\033[1;34m"
#define DARY_GRAY       "\033[1;30m"
#define CYAN            "\033[0;36m"
#define LIGHT_CYAN      "\033[1;36m"
#define PURPLE          "\033[0;35m"
#define LIGHT_PURPLE    "\033[1;35m"
#define BROWN           "\033[0;33m"
#define YELLOW          "\033[1;33m"
#define LIGHT_GRAY      "\033[0;37m"
#define WHITE           "\033[1;37m"

#define LOG_COLOR_E LIGHT_RED
#define LOG_COLOR_W YELLOW
#define LOG_COLOR_I LIGHT_GREEN
#define LOG_COLOR_D WHITE
#define LOG_COLOR_END NONE

enum {
    LOG_DUMP = 0,
    LOG_FATAL = 1,
    LOG_ERROR = 2,
    LOG_WARN = 3,
    LOG_INFO = 4,
    LOG_DEBUG = 5,
};

// return val
// 1: filename length great than max length
// 2: loglevel error
// 3: file open error
static inline mg_int_t log_init(mg_char_t* filename, mg_int_t level);

static inline void log_uninit();

static inline void log_doit(FILE* fp, mg_char_t* fmt, va_list ap);

static inline void log_debug(mg_char_t* fmt, ...);

static inline void log_info(mg_char_t* fmt, ...);

static inline void log_warning(mg_char_t* fmt, ...);

static inline void log_error(mg_char_t* fmt, ...);

static inline void log_fatal(mg_char_t* fmt, ...);

static inline void log_dump(mg_char_t* fmt, ...);

#define mglog_debug(fmt, ...)     \
    log_debug("%s [debug] %s %s %s "##fmt##" \n", mg_logtime(), __FILE__, __LINE__, __func__, __VA_ARGS__)

#define mglog_info(fmt, ...)     \
    log_info("%s [info] %s %s %s "##fmt##" \n", mg_logtime(), __FILE__, __LINE__, __func__, __VA_ARGS__)

#define mglog_waring(fmt, ...)     \
    log_warning("%s [warn] %s %s %s "##fmt##" \n", mg_logtime(), __FILE__, __LINE__, __func__, __VA_ARGS__)

#define mglog_error(fmt, ...)     \
    log_error("%s [error] %s %s %s "##fmt##" \n", mg_logtime(), __FILE__, __LINE__, __func__, __VA_ARGS__)

#define mglog_fatal(fmt, ...)     \
    log_fatal("%s [fatal] %s %s %s "##fmt##" \n", mg_logtime(), __FILE__, __LINE__, __func__, __VA_ARGS__)

#define mglog_dump(fmt, ...)     \
    log_dump("%s [dump] %s %s %s "##fmt##" \n", mg_logtime(), __FILE__, __LINE__, __func__, __VA_ARGS__)

#endif
