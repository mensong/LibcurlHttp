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

find "失败 1 个" %logfile%
if %errorlevel% equ 0 (
goto :error
)

find "失败 2 个" %logfile%
if %errorlevel% equ 0 (
goto :error
)

find "失败 3 个" %logfile%
if %errorlevel% equ 0 (
goto :error
)

find "失败 4 个" %logfile%
if %errorlevel% equ 0 (
goto :error
)

find "失败 5 个" %logfile%
if %errorlevel% equ 0 (
goto :error
)

find "失败 6 个" %logfile%
if %errorlevel% equ 0 (
goto :error
)

find "失败 7 个" %logfile%
if %errorlevel% equ 0 (
goto :error
)

find "失败 8 个" %logfile%
if %errorlevel% equ 0 (
goto :error
)

find "失败 9 个" %logfile%
if %errorlevel% equ 0 (
goto :error
)

find "失败 10 个" %logfile%
if %errorlevel% equ 0 (
goto :error
)

:sucess
goto finally

:error
echo 生成有错误，请查看%logfile%
notepad %logfile%
goto finally

:finally
exit