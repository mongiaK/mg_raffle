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

#define MAX_LOG_LINE    4096

static _mg_inline _mg_void_t log_doit(FILE* fp, _mg_char_t* fmt, va_list ap) {
    _mg_char_t buf[MAX_LOG_LINE] = {0};
    strcpy(buf, mg_logtime());
    vsprintf(buf + strlen(buf), fmt, ap);

    fprintf(fp, "%s\n", buf);
}

static _mg_void_t log_report(FILE* fp, _mg_char_t* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    log_doit(fp, fmt, ap);
    va_end(ap);
}

static _mg_void_t log_exit(FILE* fp, _mg_char_t* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    log_doit(fp, fmt, ap);
    va_end(ap);
    exit(1);
}

static _mg_void_t log_dump(FILE* fp, _mg_char_t* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    log_doit(fp, fmt, ap);
    va_end(ap);
    abort();
    exit(1);
}

#endif
