#!/bin/bash
set -ex
if [[ "$ENABLE_MAINTAINER_ZTS" == 1 ]]; then
	TS="--enable-maintainer-zts";
else
	TS="";
fi
if [[ "$ENABLE_DEBUG" == 1 ]]; then
	DEBUG="--enable-debug --without-pcre-valgrind";
else
	DEBUG="";
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
if [[ -n "$LIBMYSQL" ]]; then
	case "$LIBMYSQL" in
	*.gz) EXTRACT=zxf ;;
	*.xz) EXTRACT=Jxf ;;
	esac
	MYSQL_BASE=${LIBMYSQL%%-linux-*}
	MYSQL_VERSION=${MYSQL_BASE#*-}
	MYSQL_DIR=$HOME/$MYSQL_BASE
        mkdir -p $MYSQL_DIR
	URL=https://cdn.mysql.com//Downloads/MySQL-${MYSQL_VERSION%.*}/$LIBMYSQL
	wget $URL -O - | tar -${EXTRACT} - --strip-components=1  -C $MYSQL_DIR
fi
if [[ -z "$MYSQL_DIR" ]]; then
        PDOMYSQL=mysqlnd
        MYSQLI=mysqlnd
else
        PDOMYSQL=${MYSQL_DIR}
        [ -f ${MYSQL_DIR}/bin/mysql_config ] && MYSQLI=${MYSQL_DIR}/bin/mysql_config
fi
MAKE_JOBS=${MAKE_JOBS:-2}

if [ ! -f ${TRAVIS_BUILD_DIR}/configure ]; then
        (cd ${TRAVIS_BUILD_DIR} ; ./buildconf --force )
fi

${TRAVIS_BUILD_DIR}/configure \
--prefix="$HOME"/php-install \
$CONFIG_QUIET \
$DEBUG \
$TS \
--enable-phpdbg \
--enable-fpm \
--with-pdo-mysql=${PDOMYSQL} \
--with-mysqli=${MYSQLI} \
--with-pgsql \
--with-pdo-pgsql \
--with-pdo-sqlite \
--enable-intl \
--without-pear \
--with-gd \
--with-jpeg-dir=/usr \
--with-png-dir=/usr \
--enable-exif \
--enable-zip \
--without-libzip \
--with-zlib \
--with-zlib-dir=/usr \
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
--enable-bcmath \
--enable-calendar \
--enable-ftp \
--with-pspell=/usr \
--with-enchant=/usr \
--enable-wddx \
--with-freetype-dir=/usr \
--with-xpm-dir=/usr \
--with-kerberos \
--enable-sysvmsg \
--enable-zend-test=shared \
> "$CONFIG_LOG_FILE" || (cat config.log; exit 2)

make "-j${MAKE_JOBS}" $MAKE_QUIET > "$MAKE_LOG_FILE"
make install >> "$MAKE_LOG_FILE"
