@echo off
cd /d "%~dp0"

::定位WINDDK目录.
if "%DDKDIR%"=="" for %%A in (C D E F) do if exist %%A:\WinDDK\7600.16385.1\ set DDKDIR=%%A:\WinDDK\7600.16385.1\

::设置源码目录
set source=%~dp0

::调用编译模块,编译不同的版本.
call :build %*
pause >NUL
goto :eof

:build
setlocal
call %DDKDIR%bin\setenv.bat %DDKDIR% %*
IF NOT DEFINED MINWIN_SDK_LIB_PATH SET MINWIN_SDK_LIB_PATH=%SDK_LIB_PATH%
cd /d "%source%"
if not exist build mkdir build
build.exe /g /w /M 2 /c /F /jpath build
rd /S /Q mspack\obj%BUILD_ALT_DIR% native\obj%BUILD_ALT_DIR%
