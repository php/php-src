FROM debian:bullseye

# Install build dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    wget \
    tar \
    pkg-config \
    libtool \
    automake \
    autoconf \
    unzip \
    bison \
    re2c \
    libxml2-dev \
    libsqlite3-dev \
    libonig-dev \
    libzip-dev \
    g++ \
    locales \
    libiconv-hook-dev || true \
 && rm -rf /var/lib/apt/lists/*

RUN apt-get install -y pkg-config build-essential autoconf bison re2c libxml2-dev libsqlite3-dev

WORKDIR /tmp
RUN wget https://github.com/unicode-org/icu/releases/download/release-62-2/icu4c-62_2-src.tgz && \
    tar -xzf icu4c-62_2-src.tgz && \
    cd icu/source && \
    ./configure --prefix=/usr/local && \
    make -j$(nproc) && \
    make install && \
    ldconfig

WORKDIR /app
COPY . /app
RUN ./buildconf --force
RUN ./configure --enable-debug --enable-intl CFLAGS="-g -O0"
RUN make -j4
