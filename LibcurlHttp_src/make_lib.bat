@ECHO OFF
SETLOCAL ENABLEEXTENSIONS

if not DEFINED LibcurlHttp ( goto :err )

cd /d %~dp0

set outDir=%LibcurlHttp%\..\
rd /S /Q "%outDir%\inc\"
rd /S /Q "%outDir%\Win32\"
rd /S /Q "%outDir%\x64\"

xcopy /Y /V ".\LibcurlHttp\LibcurlHttp.h" "%outDir%\inc\"

xcopy /Y /V /E ".\Win32\*.*" "%outDir%\Win32\"

xcopy /Y /V /E ".\x64\*.*" "%outDir%\x64\"

del "%outDir%\Win32\Debug\*.exp"
del "%outDir%\Win32\Debug\*.ilk"
del "%outDir%\Win32\Debug\*.iobj"
del "%outDir%\Win32\Debug\*.ipdb"
del "%outDir%\Win32\Release\*.exp"
del "%outDir%\Win32\Release\*.ilk"
del "%outDir%\Win32\Release\*.iobj"
del "%outDir%\Win32\Release\*.ipdb"
del "%outDir%\x64\Debug\*.exp"
del "%outDir%\x64\Debug\*.ilk"
del "%outDir%\x64\Debug\*.iobj"
del "%outDir%\x64\Debug\*.ipdb"
del "%outDir%\x64\Release\*.exp"
del "%outDir%\x64\Release\*.ilk"
del "%outDir%\x64\Release\*.iobj"
del "%outDir%\x64\Release\*.ipdb"

del "%outDir%\Win32\Debug\Test.exe"
del "%outDir%\Win32\Release\Test.exe"
del "%outDir%\x64\Debug\Test.exe"
del "%outDir%\x64\Release\Test.exe"
del "%outDir%\Win32\Debug\Test.pdb"
del "%outDir%\Win32\Release\Test.pdb"
del "%outDir%\x64\Debug\Test.pdb"
del "%outDir%\x64\Release\Test.pdb"
goto :end

:err
echo 请先运行对应工具下的“设置环境变量.bat”。

:end
pause