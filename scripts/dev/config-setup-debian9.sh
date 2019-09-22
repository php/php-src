#!/bin/sh -eu

sudo apt-get update
sudo apt-get -y install \
  build-essential \
  autoconf \
  bison \
  re2c \
  pkg-config \
  libxml2-dev \
  libsqlite3-dev \
  libkrb5-dev \
  libpng-dev \
  libwebp-dev \
  libjpeg-dev \
  libfreetype6-dev \
  libc-client-dev \
  libbz2-dev \
  libenchant-dev \
  libgmp-dev \
  libldap2-dev \
  libonig-dev \
  libsystemd-dev \
  liblmdb-dev \
  libtokyocabinet-dev \
  libgdbm-dev \
  libxpm-dev \
  libsasl2-dev \
  libpq-dev \
  libpspell-dev \
  libedit-dev \
  libsnmp-dev \
  libsodium-dev \
  libtidy-dev \
  libxslt1-dev \
  libzip-dev \
  libcurl4-openssl-dev \
  libffi-dev

cd "${0%/*}/../.."

./buildconf

./configure \
  --enable-debug \
  --enable-fpm \
  --with-fpm-systemd \
  --enable-phpdbg \
  --enable-phpdbg-webhelper \
  --enable-phpdbg-readline \
  --with-openssl \
  --with-kerberos \
  --with-system-ciphers \
  --with-pcre-jit \
  --with-zlib \
  --enable-bcmath \
  --with-bz2 \
  --enable-calendar \
  --with-curl \
  --enable-dba \
  --with-lmdb  \
  --with-gdbm \
  --with-tcadb \
  --with-enchant \
  --enable-exif \
  --with-ffi \
  --enable-ftp \
  --enable-gd \
  --with-webp \
  --with-jpeg \
  --with-xpm \
  --with-freetype \
  --with-gettext \
  --with-gmp \
  --with-mhash \
  --with-imap \
  --with-kerberos \
  --with-imap-ssl \
  --enable-intl \
  --with-ldap \
  --with-ldap-sasl \
  --enable-mbstring \
  --with-mysqli \
  --enable-pcntl \
  --with-pdo-mysql \
  --with-pdo-pgsql \
  --with-pgsql \
  --with-pspell \
  --with-libedit \
  --with-readline \
  --enable-shmop \
  --with-snmp \
  --enable-soap \
  --enable-sockets \
  --with-sodium \
  --enable-sysvmsg \
  --enable-sysvsem \
  --enable-sysvshm \
  --with-tidy \
  --with-xsl \
  --with-zip \
  --enable-mysqlnd \
  --with-xmlrpc \
  --with-pear \
  --enable-werror \
  --enable-phar \
  --enable-zend-test=shared
  "$@"

