/*================================================================
*  
*  文件名称：args.h
*  创 建 者: mongia
*  创建日期：2021年03月22日
*  
================================================================*/

#pragma once

#include <stdlib.h>
#include <getopt.h>
#include <limits.h>
#include <string.h>

// copy from ovs
char* cmd_long_options_to_short_options(const struct option options[])
{
    char short_options[UCHAR_MAX * 3 + 1];
    char *p = short_options;

    for (; options->name; options++) {
        const struct option *o = options;
        if (o->flag == NULL && o->val > 0 && o->val <= UCHAR_MAX) {
            *p++ = o->val;
            if (o->has_arg == required_argument) {
                *p++ = ':';
            }   else if (o->has_arg == optional_argument) {
                *p++ = ':';
                *p++ = ':';
            }    
        }
    }
    *p++ = '\0';

    return strdup(short_options);
}
