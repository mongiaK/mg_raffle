/*================================================================
*  
*  文件名称：main.cpp
*  创 建 者: mongia
*  创建日期：2021年04月01日
*  
================================================================*/

#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unbound.h>

#include <iostream>

#include "args.h"
#include "log.h" 

static ub_ctx* _gub_ctx;

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

void dns_resolved_init()
{
    _gub_ctx = ub_ctx_create();
    if (NULL == _gub_ctx) {
        err_sys_quit("call ub_ctx_create failed");
    }

    if(ub_ctx_resolvconf(_gub_ctx, NULL) != 0) {
        ub_ctx_delete(_gub_ctx);
        err_sys_quit("read /etc/resolv.conf failed");
    }

    if(ub_ctx_hosts(_gub_ctx, NULL) != 0) {
        err_sys_report("read /etc/hosts failed");
    }

    ub_ctx_async(_gub_ctx, true);
}

void dns_resolved_destroy()
{
    ub_ctx_delete(_gub_ctx);
}

void test()
{
    char* name = "localhost";
    char addr[128];

}

int main(int argc, char* argv[])
{
    parse_options(argc, argv);

    dns_resolved_init();

    test();

    dns_resolved_destroy();

    return 0;
}
