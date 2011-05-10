@ECHO OFF
SET ROOTDIR=%~DP0
SET ROOTDIR=%ROOTDIR:~0,-8%


IF NOT DEFINED TYPE SET TYPE=chk
IF NOT DEFINED ARCH SET ARCH=x86
IF NOT DEFINED VER  SET VER=wxp

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
	ECHO δ�ҵ�DDK��װ·��,��ָ��:
	SET /P DDKDIR=
	GOTO :1
)
IF NOT EXIST %DDKDIR%bin\setenv.bat (
	ECHO DDK��װ·����Ч,������ָ��:
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

	BUILD.EXE /b /Z /g /w /M 2 /c /f /JPATH build /J build
GOTO :EOF
