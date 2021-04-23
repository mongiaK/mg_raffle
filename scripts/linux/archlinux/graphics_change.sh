#!/bin/bash

#=================================================================
#  
#  文件名称：graphics_change.sh
#  创 建 者: mongia
#  创建日期：2021年02月09日
#  
#=================================================================

arg=$1

function query_graphic_info
{
    lspci -k | grep -A 2 -E "(VGA|3D)"
}

case $arg in
    'on') 
        tee /proc/acpi/bbswitch <<< "ON"
        ;;
    'off')
        rmmod nvidia
        tee /proc/acpi/bbswitch <<< "OFF"
        ;;
    *)
        echo 'args error'
        ;;
esac
