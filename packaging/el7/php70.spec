# PHP 7.0.33 + backported security fixes, Apache mod_php for CentOS 7 (el7)
#
# Source0 is produced from the PHP-7.0.33-sec branch by build.sh
# (git archive), it contains:
#   GHSA-w8hw-w9fx-j79w (CVE-2026-109618)
#   GHSA-6wmf-4rv5-p3rp (CVE-2026-109619)

%global apiver      20151012
%global zendver     20151012
%global php_extdir  %{_libdir}/php/modules
%global httpd_moddir %{_libdir}/httpd/modules

Name:           php70
Version:        7.0.33
Release:        1.sec1%{?dist}
Summary:        PHP 7.0 scripting language with Apache mod_php (security backports)
License:        PHP and Zend and BSD
URL:            https://www.php.net/
Source0:        php-%{version}-sec.tar.gz
Source1:        php.conf
Source2:        10-php.conf
Source3:        10-opcache.ini

BuildRequires:  gcc, gcc-c++, make, autoconf, automake, libtool, bison, re2c
BuildRequires:  httpd-devel >= 2.4
BuildRequires:  libxml2-devel, libxslt-devel, openssl-devel, bzip2-devel, libcurl-devel
BuildRequires:  libjpeg-turbo-devel, libpng-devel, freetype-devel, libwebp-devel, libXpm-devel
BuildRequires:  gettext-devel, libmcrypt-devel, sqlite-devel, readline-devel, zlib-devel
BuildRequires:  pcre-devel

Requires:       httpd-mmn = %(cat %{_includedir}/httpd/.mmn 2>/dev/null || echo missing-httpd-devel)
Requires(pre):  httpd
Provides:       php = %{version}-%{release}, php(language) = %{version}
Provides:       php-cli = %{version}-%{release}, php-common = %{version}-%{release}
Provides:       mod_php = %{version}-%{release}
Provides:       php(api) = %{apiver}, php(zend-abi) = %{zendver}
Conflicts:      php < 7.0, php-common < 7.0, php-cli < 7.0
Obsoletes:      php70-mod_php < %{version}-%{release}

%description
PHP 7.0.33 built as Apache 2.4 module (mod_php, NTS, for the prefork MPM)
and CLI, with security fixes backported from PHP 8.2.35:
GHSA-w8hw-w9fx-j79w (CVE-2026-109618) and GHSA-6wmf-4rv5-p3rp
(CVE-2026-109619).

Built-in extensions: bcmath bz2 calendar ctype curl date dom exif fileinfo
filter ftp gd gettext hash iconv json libxml mbstring mcrypt mysqli mysqlnd
openssl pcntl pcre PDO pdo_mysql pdo_sqlite Phar posix readline Reflection
session SimpleXML sockets SPL sqlite3 standard tokenizer wddx xml xmlreader
xmlwriter xsl zip zlib. Zend OPcache is shared (/etc/php.d/10-opcache.ini).

%package devel
Summary:        Files needed for building PHP extensions
Requires:       %{name} = %{version}-%{release}
Requires:       autoconf, automake, gcc, make, pcre-devel
Provides:       php-devel = %{version}-%{release}
Conflicts:      php-devel < 7.0

%description devel
Header files and phpize/php-config for building PHP 7.0 extensions.

%prep
%setup -q -n php-%{version}-sec

%build
./buildconf --force
export EXTENSION_DIR=%{php_extdir}
%configure \
    --libdir=%{_libdir}/php \
    --with-libdir=%{_lib} \
    --with-config-file-path=%{_sysconfdir} \
    --with-config-file-scan-dir=%{_sysconfdir}/php.d \
    --with-apxs2=%{_bindir}/apxs \
    --disable-cgi \
    --disable-debug \
    --without-pear \
    --disable-rpath \
    --with-pic \
    --enable-bcmath \
    --with-bz2 \
    --enable-calendar \
    --enable-ctype \
    --with-curl \
    --enable-dom \
    --enable-exif \
    --enable-fileinfo \
    --enable-filter \
    --enable-ftp \
    --with-gd --with-jpeg-dir=%{_prefix} --with-png-dir=%{_prefix} \
    --with-freetype-dir=%{_prefix} --with-webp-dir=%{_prefix} --with-xpm-dir=%{_prefix} \
    --with-gettext \
    --enable-hash \
    --with-iconv \
    --enable-json \
    --enable-libxml \
    --enable-mbstring \
    --with-mcrypt \
    --enable-mysqlnd \
    --with-mysqli=mysqlnd \
    --with-pdo-mysql=mysqlnd \
    --with-mysql-sock=/var/lib/mysql/mysql.sock \
    --with-openssl \
    --enable-pcntl \
    --with-pcre-regex \
    --enable-pdo \
    --with-pdo-sqlite=%{_prefix} \
    --with-sqlite3=%{_prefix} \
    --enable-phar \
    --enable-posix \
    --with-readline \
    --enable-session \
    --enable-simplexml \
    --enable-sockets \
    --enable-tokenizer \
    --enable-wddx \
    --enable-xml \
    --enable-xmlreader \
    --enable-xmlwriter \
    --with-xsl \
    --enable-opcache \
    --enable-zip \
    --with-zlib
make %{?_smp_mflags}

%check
# Smoke tests: the binary runs and memory_limit errors terminate the
# process (see Zend/zend_alloc.c zend_mm_safe_error())
sapi/cli/php -n -v
sapi/cli/php -n -r 'exit(extension_loaded("session") ? 0 : 1);'
set +e
sapi/cli/php -n -d memory_limit=8M -r 'register_shutdown_function(function(){echo "shutdown\n";}); str_repeat("x", 32*1024*1024);' > oom.out 2>&1
rc=$?
set -e
test $rc -eq 255
grep -q 'Allowed memory size' oom.out
! grep -q shutdown oom.out

%install
# apxs -i would also edit httpd.conf; install the module by hand instead
mkdir -p %{buildroot}%{httpd_moddir}
make install-cli install-build install-headers install-programs install-modules \
    INSTALL_ROOT=%{buildroot}
install -m 755 libs/libphp7.so %{buildroot}%{httpd_moddir}/libphp7.so

install -D -m 644 php.ini-production %{buildroot}%{_sysconfdir}/php.ini
sed -i -e 's|^;\?session.save_path = .*|session.save_path = "/var/lib/php/session"|' \
    %{buildroot}%{_sysconfdir}/php.ini
mkdir -p %{buildroot}%{_sysconfdir}/php.d
install -m 644 %{SOURCE3} %{buildroot}%{_sysconfdir}/php.d/10-opcache.ini

install -D -m 644 %{SOURCE1} %{buildroot}%{_sysconfdir}/httpd/conf.d/php.conf
install -D -m 644 %{SOURCE2} %{buildroot}%{_sysconfdir}/httpd/conf.modules.d/10-php.conf

mkdir -p %{buildroot}%{_localstatedir}/lib/php/session
mkdir -p %{buildroot}%{_localstatedir}/lib/php/wsdlcache
mkdir -p %{buildroot}%{_localstatedir}/lib/php/opcache

rm -f %{buildroot}%{php_extdir}/*.a %{buildroot}%{php_extdir}/*.la

%files
%license LICENSE
%config(noreplace) %{_sysconfdir}/php.ini
%dir %{_sysconfdir}/php.d
%config(noreplace) %{_sysconfdir}/php.d/10-opcache.ini
%config(noreplace) %{_sysconfdir}/httpd/conf.d/php.conf
%config(noreplace) %{_sysconfdir}/httpd/conf.modules.d/10-php.conf
%{_bindir}/php
%{_mandir}/man1/php.1*
%{httpd_moddir}/libphp7.so
%dir %{_libdir}/php
%dir %{php_extdir}
%{php_extdir}/opcache.so
%dir %{_localstatedir}/lib/php
%attr(0770,root,apache) %dir %{_localstatedir}/lib/php/session
%attr(0770,root,apache) %dir %{_localstatedir}/lib/php/wsdlcache
%attr(0770,root,apache) %dir %{_localstatedir}/lib/php/opcache

%files devel
%{_bindir}/php-config
%{_bindir}/phpize
%{_includedir}/php
%{_libdir}/php/build
%{_mandir}/man1/php-config.1*
%{_mandir}/man1/phpize.1*

%changelog
* Wed Oct 21 2026 Umut KIRGOZ <umut.kirgoz@mynetgroup.com> - 7.0.33-1.sec1
- Backport GHSA-w8hw-w9fx-j79w (CVE-2026-109618) and
  GHSA-6wmf-4rv5-p3rp (CVE-2026-109619) from upstream 01a40e89fbf
