@echo off

set mod=%1

if "%mod%"=="wsl2" goto wsl2
if "%mod%"=="vmware" goto vmware

@echo "usage: ./wsl2_vmware_switch.bat wsl2/vmware"

goto end

:wsl2
@bcdedit /set hypervisorlaunchtype auto
goto end

:vmware
@bcdedit /set hypervisorlaunchtype off
goto end

:end
pause