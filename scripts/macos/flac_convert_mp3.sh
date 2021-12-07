#!/bin/bash

#=================================================================
#  
#  文件名称：flac_convert_mp3.sh
#  创 建 者: mongia
#  创建日期：2021年12月07日
#  
#=================================================================

# set -x

usage()
{
    echo "example ./flac_convert_mp3.sh ~/music"
}

check_input()
{
    if [[ $# -ne 1 ]]; then
        usage
        exit 1
    fi
}

main()
{
    check_input $@
    dir=$1
    if [[ ! -d $1 ]]; then
        echo "please input dir"
        exit 1
    fi

    tmpdir="/tmp/"`date +%s`
    mkdir -p tmpdir

    pushd tmpdir
    for name in `find $dir -name "*.flac"`; do
        rawName=${name##*/}
        echo ${rawName%.*}
        afconvert -f
        
    done
    
    popd
}

main $@
