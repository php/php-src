#!/bin/bash
if [[ "$ENABLE_MAINTAINER_ZTS" == 1 ]]; then
	TS="--enable-maintainer-zts";
else
	TS="";
fi

if [[ "$ENABLE_DEBUG" == 1 ]]; then
	DEBUG="--enable-debug";
else
	DEBUG="";
fi

if [[ "$TRAVIS_OS_NAME" == "osx" ]]; then
	HOMEBREW_PATH="$(brew --prefix)";
	export YACC="$(brew --prefix bison27)/bin/yacc";
	export PATH="$HOMEBREW_PATH/bin:$PATH";
	export LD_LIBRARY_PATH="$HOMEBREW_PATH/lib:$LD_LIBRARY_PATH";
	export DYLD_FALLBACK_LIBRARY_PATH="$HOMEBREW_PATH/lib:$DYLD_FALLBACK_LIBRARY_PATH";
	export C_INCLUDE_PATH="$HOMEBREW_PATH/include:$C_INCLUDE_PATH";
	export CPLUS_INCLUDE_PATH="$HOMEBREW_PATH/include:$CPLUS_INCLUDE_PATH";
	if [[ "$CC" = "gcc" ]]; then
		export CXX=g++-4.8
		export CC=gcc-4.8
	fi
fi

./buildconf --force
./configure --quiet \
$DEBUG \
$TS \
--enable-fpm \
--with-pdo-mysql=mysqlnd \
--with-mysql=mysqlnd \
--with-mysqli=mysqlnd \
--with-pgsql \
--with-pdo-pgsql \
--with-pdo-sqlite \
--enable-intl \
--with-pear \
--with-gd \
--with-jpeg-dir=/usr \
--with-png-dir=/usr \
--enable-exif \
--enable-zip \
--with-zlib \
--with-zlib-dir=/usr \
--with-mcrypt=/usr \
--enable-soap \
--enable-xmlreader \
--with-xsl \
--with-curl=/usr \
--with-tidy \
--with-xmlrpc \
--enable-sysvsem \
--enable-sysvshm \
--enable-shmop \
--enable-pcntl \
--with-readline \
--enable-mbstring \
--with-curl \
--with-gettext \
--enable-sockets \
--with-bz2 \
--with-openssl \
--with-gmp \
--enable-bcmath
make -j2 --quiet
sudo make install
