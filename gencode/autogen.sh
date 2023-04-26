#!/bin/bash

#=================================================================
#  
#  文件名称：autogen.sh
#  创 建 者: mongia
#  创建日期：2021年04月01日
#  
#=================================================================

set -e -x

if [ $# != 2 ]; then
    echo "input param error!!!!!\n\n"
    echo "usage autogen.sh cpp|go name"
    exit 0
fi

language=$1
object=$2
time=`date +%Y年%m月%d日`
author=mongia

function gencppcode
{
cat > main.cpp << EOF
/*================================================================
*  
*  文件名称：main.cpp
*  创 建 者: ${author}
*  创建日期：${time}
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
EOF
}

function genmakefile
{
cat > makefile << EOF
target:=${object}

CXX:=clang++

INCS=-I/usr/local/include
CFLAGS=-g -Wall -O0 -std=c++11

LIB_INCS=-L//usr/local/lib
STATIC_LIBS=
DYNAMIC_LIBS=

OBJ_DIR=obj

DIRS := \$(shell find . -maxdepth 5 -type d)
VPATH = \${DIRS}

SRC=\$(foreach dir, \$(DIRS), \$(wildcard \$(dir)/*.cpp))
OBJ=\$(addprefix \$(OBJ_DIR)/,\$(patsubst %.cpp,%.o,\$(notdir \$(SRC))))

all: prepare \$(target)

prepare:
	@mkdir -p \${OBJ_DIR}

\$(target): \${OBJ}
	\${CXX} \${CFLAGS} \${INCS} $^ -o \$@ \${LIB_INCS} -Wl,-Bstatic \${STATIC_LIBS} -Wl,-Bdynamic \${DYNAMIC_LIBS}

\$(OBJ_DIR)/%.o: %.cpp
	\${CXX} \${CFLAGS} \${INCS} -c $< -o \$@

.PHONY: clean

clean:
	@rm -rf \${OBJ_DIR} \${target}
EOF
}

function gencpp
{
    mkdir -p $object

    pushd $object

    gencppcode

    genmakefile

    popd
}

function gengo
{
    echo "gengo"
}

case $language in
    'cpp')
        gencpp
        ;;
    'go')
        gengo
        ;;
    *)
        ;;
esac
