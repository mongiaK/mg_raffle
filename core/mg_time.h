/*================================================================
*
*  文件名称：mg_time.h
*  创 建 者: mongia
*  创建日期：2022年01月25日
*  邮    箱：mr_pengmj@outlook.com
*
================================================================*/
#ifndef __MG_TIME_INCLUDE_H__
#define __MG_TIME_INCLUDE_H__

#include "mg_core.h"

static _mg_inline _mg_time_t mg_utime() {
    struct timeval tv;
    _mg_gettimeofday(&tv);
    return (tv.tv_sec * 1000000LL + tv.tv_usec);
}

static _mg_inline _mg_time_t mg_mtime() {
    struct timeval tv;
    _mg_gettimeofday(&tv);
    return (tv.tv_sec * 1000LL + tv.tv_usec);
}

static _mg_inline _mg_time_t mg_time() {
    return time(nullptr);
}

static _mg_inline _mg_char_t* mg_logtime() {
    static char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                             "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    static char str[32];

    _mg_tm_t *tmp;
    struct timeval tv;

    _mg_gettimeofday(&tv);
    tmp = localtime(&tv.tv_sec);

    sprintf(str, "[%02d/%s/%d:%02d:%02d:%02d] ", tmp->tm_mday,
            months[tmp->tm_mon], 1900 + tmp->tm_year, tmp->tm_hour, tmp->tm_min,
            tmp->tm_sec);
    return str;
}
#endif
