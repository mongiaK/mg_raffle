#!/bin/bash

#=================================================================
#  
#  文件名称：graphics_change.sh
#  创 建 者: mongia
#  创建日期：2021年02月09日
#  
#=================================================================

arg=$1

case $arg in
    'on') 
        tee /proc/acpi/bbswitch <<< "ON"
        ;;
    'off')
        tee /proc/acpi/bbswitch <<< "OFF"
        ;;
    *)
        echo 'args error'
        ;;
esac
