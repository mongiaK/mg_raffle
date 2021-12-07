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

#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include <iostream>

#include "log.h"
#include "args.h"

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
CXX:=clang++

INCS=-I ../../utils

CFLAGS=-g -Wall -O0 -std=c++11
LIBS=

SRC=\${wildcard *.cpp}
OBJ=\${patsubst %.cpp,%.o,\${SRC}}

.PHONY: clean

all: ${object}

${object}: main.o
	\${CXX} \${CFLAGS} \${INCS} $^ -o \$@ \${LIBS}

%.o:%.cpp
	\${CXX} \${CFLAGS} \${INCS} -c $< -o \$@

clean:
	@rm -f \${OBJ} ${object}
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
