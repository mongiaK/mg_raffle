/*================================================================
*
*  文件名称：mg_log.h
*  创 建 者: mongia
*  创建日期：2021年04月01日
*
================================================================*/
#ifndef __MG_LOG_INCLUDE_H__
#define __MG_LOG_INCLUDE_H__

#include "mg_config.h"
#include "mg_core.h"

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

#define MAX_LOG_FILENAME    4096
#define LOG_CHECK(x)    \
    if (glog_level < x)  \
    return

enum {
    LOG_DUMP = 0,
    LOG_FATAL = 1,
    LOG_ERROR = 2,
    LOG_WARN = 3,
    LOG_INFO = 4,
    LOG_DEBUG = 5,
};

static FILE* glogfp = stdout;
static mg_char_t glog_filename[MAX_LOG_FILENAME] = {0};
#ifdef MG_DEBUG
static mg_int_t glog_level = LOG_DEBUG;
#else
static mg_int_t glog_level = LOG_WARN;
#endif

// return val
// 1: filename length great than max length
// 2: loglevel error
// 3: file open error
static mg_inline mg_int_t log_init(mg_char_t* filename, mg_int_t level) {
    FILE* fp;
    mg_int_t n, length;

    length = strlen(filename);
    if (length > MAX_LOG_FILENAME) {
        return 1;
    }

    if (level < LOG_DUMP || level > LOG_DEBUG) {
        return 2;
    }
    
    fp = fopen(filename, "ab+");
    if (fp == nullptr) {
        return 3;
    }

    n = sprintf(glog_filename, "%s", filename);
    glog_filename[n] = '\0';

    glogfp = fp;
    glog_level = level;

    return 0;
}

static mg_inline mg_void_t log_uninit() {
    if (glogfp != nullptr) {
        fclose(glogfp);
    }
}

static mg_inline mg_void_t log_doit(FILE* fp, mg_char_t* fmt, va_list ap) {
    fprintf(fp, fmt, ap);
}

static mg_inline mg_void_t log_debug(mg_char_t* fmt, ...) {
    LOG_CHECK(LOG_DEBUG);

    va_list ap;
    va_start(ap, fmt);
    log_doit(glogfp, fmt, ap);
    va_end(ap);
}

static mg_inline mg_void_t log_info(mg_char_t* fmt, ...) {
    LOG_CHECK(LOG_INFO);

    va_list ap;
    va_start(ap, fmt);
    log_doit(glogfp, fmt, ap);
    va_end(ap);
}

static mg_inline mg_void_t log_warning(mg_char_t* fmt, ...) {
    LOG_CHECK(LOG_WARN);

    va_list ap;
    va_start(ap, fmt);
    log_doit(glogfp, fmt, ap);
    va_end(ap);
}

static mg_inline mg_void_t log_error(mg_char_t* fmt, ...) {
    LOG_CHECK(LOG_ERROR);

    va_list ap;
    va_start(ap, fmt);
    log_doit(glogfp, fmt, ap);
    va_end(ap);
}

static mg_inline mg_void_t log_fatal(mg_char_t* fmt, ...) {
    LOG_CHECK(LOG_FATAL);

    va_list ap;
    va_start(ap, fmt);
    log_doit(glogfp, fmt, ap);
    va_end(ap);
    exit(1);
}

static mg_inline mg_void_t log_dump(mg_char_t* fmt, ...) {
    LOG_CHECK(LOG_DUMP);

    va_list ap;
    va_start(ap, fmt);
    log_doit(glogfp, fmt, ap);
    va_end(ap);
    abort();
    exit(1);
}

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
