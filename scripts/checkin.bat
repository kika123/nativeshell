@ECHO OFF
CD ..
ECHO ������ע��:
SET /P MSG=
svn ci -m "MSG"
PAUSE>NUL