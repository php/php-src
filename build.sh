#!/bin/sh
brew install pkg-config \
                   autoconf \
                   bison \
                   re2c
brew install openssl@1.1 \
                   krb5 \
                   bzip2 \
                   enchant \
                   libffi \
                   libpng \
                   webp \
                   freetype \
                   intltool \
                   icu4c \
                   libiconv \
                   zlib \
                   t1lib \
                   gd \
                   libzip \
                   gmp \
                   tidyp \
                   libxml2 \
                   libxslt \
                   postgresql
brew link icu4c gettext --force

export PATH="/usr/local/opt/bison/bin:$PATH"
export PKG_CONFIG_PATH="$PKG_CONFIG_PATH:/usr/local/opt/openssl@1.1/lib/pkgconfig"
export PKG_CONFIG_PATH="$PKG_CONFIG_PATH:/usr/local/opt/krb5/lib/pkgconfig"
export PKG_CONFIG_PATH="$PKG_CONFIG_PATH:/usr/local/opt/libffi/lib/pkgconfig"
export PKG_CONFIG_PATH="$PKG_CONFIG_PATH:/usr/local/opt/libxml2/lib/pkgconfig"
export PKG_CONFIG_PATH="$PKG_CONFIG_PATH:/usr/local/opt/libxslt/lib/pkgconfig"
export PKG_CONFIG_PATH="$PKG_CONFIG_PATH:/usr/local/opt/zlib/lib/pkgconfig"
export PKG_CONFIG_PATH="$PKG_CONFIG_PATH:/usr/local/opt/icu4c/lib/pkgconfig"
./buildconf --force
./configure  \
            --enable-option-checking=fatal \
            --prefix=/usr/local \
            --disable-phpdbg \
            --enable-fpm \
            --with-pdo-mysql=mysqlnd \
            --with-mysqli=mysqlnd \
            --with-pgsql \
            --with-pdo-pgsql \
            --with-pdo-sqlite \
            --without-pear \
             \
            --with-jpeg \
            --with-webp \
            --with-freetype \
             \
            --with-zip \
            --with-zlib \
            --enable-soap \
            --enable-xmlreader \
            --with-xsl \
            --with-tidy=/usr/local/opt/tidyp \
            --with-xmlrpc \
            --with-libxml \
            --enable-sysvsem \
            --enable-sysvshm \
            --enable-shmop \
            --enable-pcntl \
            --with-readline=/usr/local/opt/readline \
            --enable-mbstring \
            --with-curl \
            --with-gettext=/usr/local/opt/gettext \
            --enable-sockets \
            --with-bz2=/usr/local/opt/bzip2 \
            --with-openssl \
            --with-gmp=/usr/local/opt/gmp \
             \
            --enable-bcmath \
            --enable-calendar \
            --enable-ftp \
            --with-pspell=/usr/local/opt/aspell \
            --with-kerberos \
            --enable-sysvmsg \
            --with-ffi \
            --enable-zend-test \
            --disable-werror \
            --with-mhash \
             \
            --with-config-file-path=/etc \
            --with-config-file-scan-dir=/etc/php.d
export PATH="/usr/local/opt/bison/bin:$PATH"
export LIBXML_CFLAGS="-I/usr/local/opt/libxml2/include"
export LIBXML_LIBS="-L/usr/local/opt/libxml2/lib"
export SQLITE_CFLAGS="-I/usr/local/opt/sqlite/include"
export SQLITE_LIBS="-L/usr/local/opt/sqlite/lib"
export FFI_LIBS="-L/usr/local/opt/libffi/lib"
export FFI_CFLAGS="-I/usr/local/opt/libffi/lib/libffi-3.2.1/include"
make -j$(sysctl -n hw.ncpu)
