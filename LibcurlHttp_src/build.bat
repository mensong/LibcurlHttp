@echo off

cd /d "%~dp0"

set logfile=build.log

echo ============================ %~n0 ============================>%logfile%

devenv.exe "LibcurlHttp.sln" /Project "LibcurlHttp" /Build "Release|x64" >>%logfile%
devenv.exe "LibcurlHttp.sln" /Project "LibcurlHttp" /Build "Debug|x64" >>%logfile%
devenv.exe "LibcurlHttp.sln" /Project "LibcurlHttp" /Build "Release|x86" >>%logfile%
devenv.exe "LibcurlHttp.sln" /Project "LibcurlHttp" /Build "Debug|x86" >>%logfile%

devenv.exe "LibcurlHttp.sln" /Project "CurlUI" /Build "Release|x64" >>%logfile%
devenv.exe "LibcurlHttp.sln" /Project "CurlUI" /Build "Debug|x64" >>%logfile%
devenv.exe "LibcurlHttp.sln" /Project "CurlUI" /Build "Release|x86" >>%logfile%
devenv.exe "LibcurlHttp.sln" /Project "CurlUI" /Build "Debug|x86" >>%logfile%

find "ʧ�� 1 ��" %logfile%
if %errorlevel% equ 0 (
goto :error
)

find "ʧ�� 2 ��" %logfile%
if %errorlevel% equ 0 (
goto :error
)

find "ʧ�� 3 ��" %logfile%
if %errorlevel% equ 0 (
goto :error
)

find "ʧ�� 4 ��" %logfile%
if %errorlevel% equ 0 (
goto :error
)

find "ʧ�� 5 ��" %logfile%
if %errorlevel% equ 0 (
goto :error
)

find "ʧ�� 6 ��" %logfile%
if %errorlevel% equ 0 (
goto :error
)

find "ʧ�� 7 ��" %logfile%
if %errorlevel% equ 0 (
goto :error
)

find "ʧ�� 8 ��" %logfile%
if %errorlevel% equ 0 (
goto :error
)

find "ʧ�� 9 ��" %logfile%
if %errorlevel% equ 0 (
goto :error
)

find "ʧ�� 10 ��" %logfile%
if %errorlevel% equ 0 (
goto :error
)

:sucess
goto finally

:error
echo �����д�����鿴%logfile%
notepad %logfile%
goto finally

:finally
exit