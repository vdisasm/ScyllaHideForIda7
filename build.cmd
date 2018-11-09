:: Variables
set plgname=ScyllaHideForIda7Plugin
set srcdir=ScyllaHideForIda7Plugin
set dstdir=built

:: Clean before build
rmdir /s /q %srcdir%\x64

rmdir /s /q %dstdir%
mkdir %dstdir%

:: Build
cd %srcdir%

call "%ProgramFiles(x86)%\Microsoft Visual Studio\2017\Enterprise\Common7\Tools\VsDevCmd.bat"

msbuild /property:Configuration="Release IDA64-32",Platform=x64
msbuild /property:Configuration="Release IDA64-64",Platform=x64

cd ..

:: Copy results
copy "%srcdir%\x64\Release IDA64-32\%plgname%.dll"    "%dstdir%\"
copy "%srcdir%\x64\Release IDA64-64\%plgname%64.dll"  "%dstdir%\"