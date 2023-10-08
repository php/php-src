#!/bin/bash
set -ex
if [[ "$ENABLE_ZTS" == 1 ]]; then
	TS="--enable-zts";
else
	TS="";
fi
if [[ "$ENABLE_DEBUG" == 1 ]]; then
	DEBUG="--enable-debug";
else
	DEBUG="";
fi
if [[ "$S390X" == 1 ]]; then
	S390X_CONFIG="--without-pcre-jit";
else
	S390X_CONFIG="";
fi

./buildconf --force
./configure \
--enable-option-checking=fatal \
--prefix="$HOME"/php-install \
$CONFIG_QUIET \
$DEBUG \
$TS \
$S390X_CONFIG \
--enable-phpdbg \
--enable-fpm \
--with-pdo-mysql=mysqlnd \
--with-mysqli=mysqlnd \
--with-pgsql \
--with-pdo-pgsql \
--with-pdo-sqlite \
--enable-intl \
--without-pear \
--enable-gd \
--with-jpeg \
--with-webp \
--with-freetype \
--with-xpm \
--enable-exif \
--with-zip \
--with-zlib \
--enable-soap \
--enable-xmlreader \
--with-xsl \
--with-tidy \
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
--enable-bcmath \
--enable-calendar \
--enable-ftp \
--with-pspell=/usr \
--with-enchant=/usr \
--with-kerberos \
--enable-sysvmsg \
--with-ffi \
--with-sodium \
--enable-zend-test=shared \
--enable-dl-test=shared \
--enable-werror \
--with-pear

if [[ -z "$CONFIG_ONLY" ]]; then
	MAKE_JOBS=${MAKE_JOBS:-$(nproc)}
	make "-j${MAKE_JOBS}" $MAKE_QUIET
	make install
fi
