@echo off
if NOT DEFINED PHP_VER set PHP_VER=8.2.5

if "%PHP_VER%" == "7.4.33" (
    set CRT=vc15
) else (
    set CRT=vs16
)

if NOT EXIST php-sdk (
  curl -L https://github.com/php/php-sdk-binary-tools/archive/refs/tags/php-sdk-2.2.0.tar.gz | tar xzf - && ren php-sdk-binary-tools-php-sdk-2.2.0 php-sdk
)

if NOT DEFINED ARCH set ARCH=x64
if NOT DEFINED PHP_TS set ZTS=--disable-zts

if NOT EXIST php-sdk\phpdev\%CRT%\%ARCH% (mkdir php-sdk\phpdev\%CRT%\%ARCH%)
if NOT EXIST php-sdk\phpdev\%CRT%\%ARCH%\php-%PHP_VER% (
	mklink /j "php-sdk\phpdev\%CRT%\%ARCH%\php-%PHP_VER%" .
)

cd php-sdk

set "VSCMD_START_DIR=%CD%"
set "__VSCMD_ARG_NO_LOGO=yes"
set PHP_SDK_ROOT_PATH=%~dp0
set PHP_SDK_ROOT_PATH=%PHP_SDK_ROOT_PATH:~0,-1%
set PHP_SDK_RUN_FROM_ROOT=.\php-sdk

copy /Y ..\cmd\phpsdk_setshell.bat bin\phpsdk_setshell.bat

bin\phpsdk_setshell.bat %CRT% %ARCH% && bin\phpsdk_setvars.bat && bin\phpsdk_dumpenv.bat && bin\phpsdk_buildtree.bat phpdev && cd php-%PHP_VER% && ..\..\..\..\bin\phpsdk_deps -u --no-backup && if EXIST config.nice.bat (
  config.nice.bat && nmake %SNAP% && cd ..\..\..\..\..
) else (
  buildconf --force --add-modules-dir=..\pecl\ && configure --enable-cli --with-ffi --with-iconv%SHARE% --enable-phar%SHARE% --enable-filter%SHARE% --with-openssl%SHARE% --enable-sockets%SHARE% --enable-mbstring%SHARE% --with-libxml%SHARE% --enable-fileinfo%SHARE% --enable-xmlwriter%SHARE% --enable-tokenizer%SHARE% --enable-embed %EXT% %ZTS% %OPTIONS% && nmake %SNAP% && cd ..\..\..\..\..
)
