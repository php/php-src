FROM alpine:3.12

RUN apk add \
    aspell-dev \
    autoconf \
    bison \
    bzip2-dev \
    curl-dev \
    freetype-dev \
    g++ \
    gcc \
    gettext-dev \
    gnu-libiconv-dev \
    gmp-dev \
    icu-dev \
    jpeg-dev \
    libffi-dev \
    libpng-dev \
    libsodium-dev \
    libwebp-dev \
    libxml2-dev \
    libxpm-dev \
    libxslt-dev \
    libzip-dev \
    make \
    oniguruma-dev \
    openssl-dev \
    pkgconf \
    re2c \
    readline-dev \
    sqlite-dev \
    tidyhtml-dev \
    tzdata

ENV SKIP_IO_CAPTURE_TESTS=1

WORKDIR /php-src
COPY . /php-src

RUN ./buildconf
RUN ./configure \
    --enable-option-checking=fatal \
    --enable-debug \
    --enable-werror \
    --enable-phpdbg \
    --enable-fpm \
    --enable-bcmath \
    --enable-calendar \
    --enable-dba \
    --enable-intl \
    --enable-exif \
    --enable-ftp \
    --enable-gd \
    --enable-mbstring \
    --enable-pcntl \
    --enable-shmop \
    --enable-soap \
    --enable-sockets \
    --enable-sysvmsg \
    --enable-sysvsem \
    --enable-sysvshm \
    --enable-xmlreader \
    --enable-zend-test \
    --with-curl \
    --with-bz2 \
    --with-iconv=/usr \
    --with-jpeg \
    --with-ffi \
    --with-freetype \
    --with-gettext \
    --with-gmp \
    --with-openssl \
    --with-pspell \
    --with-readline \
    --with-sodium \
    --with-tidy \
    --with-webp \
    --with-xpm \
    --with-xsl \
    --with-zip \
    --with-zlib

RUN make clean && make -j`nproc`
RUN sapi/cli/php run-tests.php -j`nproc` -q \
    --show-diff -g FAIL,XFAIL,BORK,WARN,LEAK,XLEAK,SKIP
