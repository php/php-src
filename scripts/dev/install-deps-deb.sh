#!/bin/sh -eu

export DEBIAN_FRONTEND=noninteractive

install_deps_deb() {
  apt_update() {
     apt-get update
  }

  apt_install() {
    apt-get install --yes "$@"
  }

  if [ "$(id -u)" -ne 0 ]; then
    printf -- '%s must be run as root\n' "$0" >&2
    exit 1
  fi

  apt_update
  apt_install lsb-release

  case "$(lsb_release -si)" in
    (Ubuntu)
      apt_install software-properties-common
      apt-add-repository -y ppa:ondrej/php
      case "$(lsb_release -sc)" in
        (xenial)
          if ! grep -r -E '^deb.*xenial-backports.*' /etc/apt/sources.list* > /dev/null; then
            printf -- '%s\n' 'deb http://archive.ubuntu.com/ubuntu/ xenial-backports main restricted universe multiverse' \
              > /etc/apt/sources.list.d/xenial-backports.list
          fi

          printf -- '%s\n' 'Package: liblmdb-dev liblmdb0 lmdb-doc' 'Pin: release a=xenial-backports' 'Pin-Priority: 500' \
            > /etc/apt/preferences.d/xenial-backports-lmdb
        ;;
      esac
      apt_update
    ;;
  esac

  apt_install \
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
    libqdbm-dev \
    libtokyocabinet-dev \
    libxpm-dev \
    libsasl2-dev \
    libpq-dev \
    libpspell-dev \
    libedit-dev \
    libsnmp-dev \
    libsodium-dev \
    libargon2-0-dev \
    libtidy-dev \
    libxslt1-dev \
    libzip-dev \
    libcurl4-openssl-dev \
    libffi-dev \
    "$@"
}

install_deps_deb "$@"
