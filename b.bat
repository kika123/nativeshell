@echo off
cd /d "%~dp0"

if defined %1 (set arg1=%1) else (set arg1=chk)
if defined %2 (set arg2=%2) else (set arg2=x86)
if defined %3 (set arg2=%3) else (set arg3=wnet)

::定位WINDDK目录.
if "%DDKDIR%"=="" for %%A in (C D E F) do if exist %%A:\WinDDK\7600.16385.1\ set DDKDIR=%%A:\WinDDK\7600.16385.1\


::设置源码目录
set source=%~dp0

::如果有存在SUBST命令,则使用SUBST虚拟一个B:
::因为如果源码目录存放的路径比较长或带空格时可能会编译失败.
::subst && set subst=b: && subst /d b: & cls

::if defined subst (subst %subst% . && set source=b:)

::调用编译模块,编译不同的版本.
call :build %arg1% %arg2% %arg3%

::取消subst映射
::if defined subst subst /d %subst%

pause
goto :eof


:build
setlocal
call %DDKDIR%bin\setenv.bat %DDKDIR% %*
IF NOT DEFINED MINWIN_SDK_LIB_PATH SET MINWIN_SDK_LIB_PATH=%SDK_LIB_PATH%
cd /d "%source%"
if not exist obj%BUILD_ALT_DIR% mkdir obj%BUILD_ALT_DIR%
build.exe /g /w /M 2 /c /F /y /jpath obj%BUILD_ALT_DIR%
