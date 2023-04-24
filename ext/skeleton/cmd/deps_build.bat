@echo off
curl -LO https://cytranet.dl.sourceforge.net/project/pthreads4w/pthreads4w-code-v3.0.0.zip
unzip -xoq pthreads4w-code-v3.0.0.zip
ren pthreads4w-code-07053a521b0a9deb6db2a649cde1f828f2eb1f4f pthreads4w
mkdir pthreads4w\cmake
copy /Y ..\..\..\..\cmd\deps_build\*.* pthreads4w\cmake
cd pthreads4w
mkdir build
cd build
cmake .. -D BUILD_TESTING=OFF -G "Visual Studio 16 2019" -A %ARCH% -D CMAKE_BUILD_TYPE=Release
cmake --build . --config Release --target install
copy /Y ..\PTHREADS-BUILT\lib\*.* ..\..\deps\lib\
copy /Y ..\PTHREADS-BUILT\include\*.* ..\..\deps\include\
copy /Y ..\PTHREADS-BUILT\bin\*.* ..\..\deps\bin\
copy /Y ..\PTHREADS-BUILT\bin\*.* ..\..\php-%PHP_VER%\%ARCH%\Release%PHP_TS%\php-%PHP_VER%
cd ..\..

del pthreads4w-code-v3.0.0.zip
rmdir /S /Q pthreads4w
