/*================================================================
*  
*  文件名称：main.cpp
*  创 建 者: mongia
*  创建日期：2023年06月14日
*  
================================================================*/

#include <bits/stdc++.h>
#include <getopt.h>

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

void parse_options(int argc, char* argv[])
{
    static const struct option long_options[] = {
        {"help", no_argument, NULL, 'h'},
        {"version", no_argument, NULL, 'v'},
        {NULL, 0, NULL, 0},
    };

    char* short_options = cmd_long_options_to_short_options(long_options);
    for(;;) {
        int c = getopt_long(argc, argv, short_options, long_options, NULL);
        if (-1 == c) {
            break;
        }

        switch(c) {
            case 'h':
                break;
            case 'v':
                break;
            default:
                abort();
        }
    }

    free(short_options);
}

int main(int argc, char* argv[])
{
    parse_options(argc, argv);

    return 0;
}
