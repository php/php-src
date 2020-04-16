#!/bin/bash
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

if [[ -z "$CONFIG_LOG_FILE" ]]; then
	CONFIG_QUIET="--quiet"
	CONFIG_LOG_FILE="/dev/stdout"
else
	CONFIG_QUIET=""
fi
if [[ -z "$MAKE_LOG_FILE" ]]; then
	MAKE_QUIET="--quiet"
	MAKE_LOG_FILE="/dev/stdout"
else
	MAKE_QUIET=""
fi

MAKE_JOBS=${MAKE_JOBS:-$(nproc)}

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
--with-zlib-dir=/usr \
--enable-soap \
--enable-xmlreader \
--with-xsl \
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
--enable-werror \
--with-pear

make "-j${MAKE_JOBS}" $MAKE_QUIET
make install
