/*================================================================
*  
*  文件名称：mg_log.c
*  创 建 者: mongia
*  创建日期：2022年03月16日
*  邮    箱：mongiaK@outlook.com
*  
================================================================*/

#include "mg_log.h"

#define MAX_LOG_FILENAME    4096
#define LOG_CHECK(x)    \
    if (glog_level < x)  \
    return

static FILE* glogfp = stdout;
static mg_char_t glog_filename[MAX_LOG_FILENAME] = {0};
#ifdef MG_DEBUG
static mg_int_t glog_level = LOG_DEBUG;
#else
static mg_int_t glog_level = LOG_WARN;
#endif

static inline mg_int_t log_init(mg_char_t* filename, mg_int_t level) {
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

static inline void log_uninit() {
    if (glogfp != nullptr) {
        fclose(glogfp);
    }
}

static inline void log_doit(FILE* fp, mg_char_t* fmt, va_list ap) {
    fprintf(fp, fmt, ap);
}

static inline void log_debug(mg_char_t* fmt, ...) {
    LOG_CHECK(LOG_DEBUG);

    va_list ap;
    va_start(ap, fmt);
    log_doit(glogfp, fmt, ap);
    va_end(ap);
}

static inline void log_info(mg_char_t* fmt, ...) {
    LOG_CHECK(LOG_INFO);

    va_list ap;
    va_start(ap, fmt);
    log_doit(glogfp, fmt, ap);
    va_end(ap);
}

static inline void log_warning(mg_char_t* fmt, ...) {
    LOG_CHECK(LOG_WARN);

    va_list ap;
    va_start(ap, fmt);
    log_doit(glogfp, fmt, ap);
    va_end(ap);
}

static inline void log_error(mg_char_t* fmt, ...) {
    LOG_CHECK(LOG_ERROR);

    va_list ap;
    va_start(ap, fmt);
    log_doit(glogfp, fmt, ap);
    va_end(ap);
}

static inline void log_fatal(mg_char_t* fmt, ...) {
    LOG_CHECK(LOG_FATAL);

    va_list ap;
    va_start(ap, fmt);
    log_doit(glogfp, fmt, ap);
    va_end(ap);
    exit(1);
}

static inline void log_dump(mg_char_t* fmt, ...) {
    LOG_CHECK(LOG_DUMP);

    va_list ap;
    va_start(ap, fmt);
    log_doit(glogfp, fmt, ap);
    va_end(ap);
    abort();
    exit(1);
}

