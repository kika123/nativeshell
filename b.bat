@echo off
cd /d "%~dp0"

::��λWINDDKĿ¼.
if "%DDKDIR%"=="" for %%A in (C D E F) do if exist %%A:\WinDDK\7600.16385.1\ set DDKDIR=%%A:\WinDDK\7600.16385.1\

::����Դ��Ŀ¼
set source=%~dp0

::���ñ���ģ��,���벻ͬ�İ汾.
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
