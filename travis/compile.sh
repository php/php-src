#!/bin/bash
./buildconf
./configure --quiet \
--with-pdo-mysql=mysqlnd \
--with-mysql=mysqlnd \
--with-mysqli=mysqlnd \
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
--enable-bcmath
make --quiet
