@ECHO OFF
SET ROOTDIR=%~DP0
SET ROOTDIR=%ROOTDIR:~0,-8%
SET THREAD=1

IF NOT DEFINED TYPE SET TYPE=chk
IF NOT DEFINED ARCH SET ARCH=x86
IF NOT DEFINED VER  SET VER=wxp
IF DEFINED NUMBER_OF_PROCESSORS  SET THREAD=%NUMBER_OF_PROCESSORS%

IF NOT DEFINED DDKDIR (
	FOR %%i IN (C D E F G H I J K L M N O P Q R S T U V W X Y Z) DO (
		IF EXIST %%i:\WinDDK\ (
			CD /D %%i:\WinDDK\
			FOR /F %%j IN ('DIR /B /A:D') DO (
				IF EXIST %%i:\WinDDK\%%j\bin\setenv.bat (
					SET DDKDIR=%%i:\WinDDK\%%j\
					GOTO :1
				)
			)
		)
	)
)

:1
IF NOT DEFINED DDKDIR (
	ECHO 未找到DDK安装路径,请指定:
	SET /P DDKDIR=
	GOTO :1
)
IF NOT EXIST %DDKDIR%bin\setenv.bat (
	ECHO DDK安装路径无效,请重新指定:
	SET /P DDKDIR=
	GOTO :1
)
CALL :BUILD
PAUSE >NUL
GOTO :EOF

:BUILD
	SETLOCAL
	CALL %DDKDIR%bin\setenv.bat %DDKDIR% %TYPE% %ARCH% %VER% NO_OACR
	IF NOT DEFINED MINWIN_SDK_LIB_PATH SET MINWIN_SDK_LIB_PATH=%SDK_LIB_PATH%
	SET BUILD_ALT_DIR=\
	CD /D "%ROOTDIR%"

	IF NOT EXIST build MKDIR build

	BUILD.EXE /b /Z /g /w /M %THREAD% /c /f /JPATH build /J build
GOTO :EOF
