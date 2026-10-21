# CentOS 7 build environment for php 7.0.33 mod_php (el7)
FROM centos:7

# CentOS 7 is EOL: point yum at vault.centos.org
RUN set -eux; \
    arch="$(uname -m)"; \
    if [ "$arch" = "x86_64" ]; then base="http://vault.centos.org/7.9.2009"; \
    else base="http://vault.centos.org/altarch/7.9.2009"; fi; \
    rm -f /etc/yum.repos.d/*.repo; \
    for r in os updates extras; do \
      printf '[%s]\nname=CentOS-7 %s\nbaseurl=%s/%s/$basearch/\ngpgcheck=0\nenabled=1\n' "$r" "$r" "$base" "$r" >> /etc/yum.repos.d/vault.repo; \
    done; \
    yum -y install epel-release || true; \
    if [ "$arch" = "x86_64" ]; then epel="https://archives.fedoraproject.org/pub/archive/epel/7/x86_64/"; \
    else epel="https://archives.fedoraproject.org/pub/archive/epel/7/aarch64/"; fi; \
    rm -f /etc/yum.repos.d/epel*.repo; \
    printf '[epel]\nname=EPEL 7\nbaseurl=%s\ngpgcheck=0\nenabled=1\n' "$epel" > /etc/yum.repos.d/epel.repo; \
    yum -y install \
      gcc gcc-c++ make autoconf automake libtool bison re2c rpm-build which file \
      httpd httpd-devel \
      libxml2-devel libxslt-devel openssl-devel bzip2-devel libcurl-devel \
      libjpeg-turbo-devel libpng-devel freetype-devel libwebp-devel libXpm-devel \
      gettext-devel libmcrypt-devel sqlite-devel readline-devel zlib-devel \
      libevent-devel libmemcached-devel hiredis-devel cyrus-sasl-devel \
      gmp-devel pcre-devel; \
    yum clean all
