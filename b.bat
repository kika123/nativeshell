@echo off
cd /d "%~dp0"

if defined %1 (set arg1=%1) else (set arg1=chk)
if defined %2 (set arg2=%2) else (set arg2=x86)
if defined %3 (set arg2=%3) else (set arg3=wnet)

::��λWINDDKĿ¼.
if "%DDKDIR%"=="" for %%A in (C D E F) do if exist %%A:\WinDDK\7600.16385.1\ set DDKDIR=%%A:\WinDDK\7600.16385.1\


::����Դ��Ŀ¼
set source=%~dp0

::����д���SUBST����,��ʹ��SUBST����һ��B:
::��Ϊ���Դ��Ŀ¼��ŵ�·���Ƚϳ�����ո�ʱ���ܻ����ʧ��.
::subst && set subst=b: && subst /d b: & cls

::if defined subst (subst %subst% . && set source=b:)

::���ñ���ģ��,���벻ͬ�İ汾.
call :build %arg1% %arg2% %arg3%

::ȡ��substӳ��
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
