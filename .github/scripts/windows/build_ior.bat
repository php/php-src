@echo off

rem Temporary, for the IO hooks proof of concept only: the ring
rem (main/io/php_io_ring.c) is built on the ior library, which will be
rem bundled with php-src once the design settles. Until then CI fetches it
rem from its repository and builds it into a prefix that configure.bat gets
rem as --with-ior. The Windows counterpart of .github/actions/build-ior.
rem
rem Runs inside the SDK shell, so cl.exe, nmake and the environment match
rem the toolset and platform of the PHP build; CMake's NMake generator picks
rem them up. Sets IOR_PREFIX for the caller.

set IOR_REF=main
set IOR_SRC=%PHP_BUILD_CACHE_BASE_DIR%\ior-src
set IOR_PREFIX=%PHP_BUILD_CACHE_BASE_DIR%\ior-%PHP_SDK_VS%-%PHP_SDK_ARCH%

if exist "%IOR_SRC%" rmdir /s /q "%IOR_SRC%"
git clone --depth 1 --branch %IOR_REF% https://github.com/libior/ior.git "%IOR_SRC%" 2>&1
if %errorlevel% neq 0 exit /b 3

cmake -S "%IOR_SRC%" -B "%IOR_SRC%\build" -G "NMake Makefiles" ^
	-DCMAKE_BUILD_TYPE=Release ^
	-DCMAKE_INSTALL_PREFIX="%IOR_PREFIX%" ^
	-DIOR_BUILD_TESTS=OFF ^
	-DIOR_BUILD_BENCH=OFF
if %errorlevel% neq 0 exit /b 3
cmake --build "%IOR_SRC%\build"
if %errorlevel% neq 0 exit /b 3
cmake --install "%IOR_SRC%\build"
if %errorlevel% neq 0 exit /b 3

echo Built ior into %IOR_PREFIX%
exit /b 0
