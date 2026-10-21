# PECL extensions for php70 (7.0.33 sec), one subpackage per extension.
# Versions are the last releases known to support PHP 7.0.

%global php_extdir  %{_libdir}/php/modules
%global php_inidir  %{_sysconfdir}/php.d

%global apcu_ver      5.1.17
%global apcu_bc_ver   1.0.5
%global event_ver     2.5.7
%global igbinary_ver  3.2.16
%global memcache_ver  4.0.5.2
%global memcached_ver 3.2.0
%global mongodb_ver   1.9.2
%global msgpack_ver   2.1.2
%global phpiredis_ver 1.0.1
%global redis_ver     5.3.7
%global xdebug_ver    2.7.2

Name:           php70-pecl
Version:        7.0.33
Release:        1.sec1%{?dist}
Summary:        PECL extensions for php70
License:        PHP and BSD and ASL 2.0
URL:            https://pecl.php.net/
Source0:        https://pecl.php.net/get/apcu-%{apcu_ver}.tgz
Source1:        https://pecl.php.net/get/apcu_bc-%{apcu_bc_ver}.tgz
Source2:        https://pecl.php.net/get/event-%{event_ver}.tgz
Source3:        https://pecl.php.net/get/igbinary-%{igbinary_ver}.tgz
Source4:        https://pecl.php.net/get/memcache-%{memcache_ver}.tgz
Source5:        https://pecl.php.net/get/memcached-%{memcached_ver}.tgz
Source6:        https://pecl.php.net/get/mongodb-%{mongodb_ver}.tgz
Source7:        https://pecl.php.net/get/msgpack-%{msgpack_ver}.tgz
Source8:        https://github.com/nrk/phpiredis/archive/refs/tags/v%{phpiredis_ver}.tar.gz#/phpiredis-%{phpiredis_ver}.tar.gz
Source9:        https://pecl.php.net/get/redis-%{redis_ver}.tgz
Source10:       https://pecl.php.net/get/xdebug-%{xdebug_ver}.tgz

BuildRequires:  php70-devel = %{version}
BuildRequires:  gcc, make, autoconf, automake, libtool
BuildRequires:  openssl-devel, zlib-devel, cyrus-sasl-devel
BuildRequires:  libevent-devel, libmemcached-devel, hiredis-devel

%description
PECL extensions built against php70 %{version}.

# ----------------------------------------------------------------------
%package -n php70-pecl-apcu
Summary:        APCu user cache (%{apcu_ver})
Version:        %{apcu_ver}
Requires:       php70 = 7.0.33
%description -n php70-pecl-apcu
APCu - APC User Cache.

%package -n php70-pecl-apcu-bc
Summary:        APCu backwards compatibility module, provides "apc" (%{apcu_bc_ver})
Version:        %{apcu_bc_ver}
Requires:       php70-pecl-apcu = %{apcu_ver}
Provides:       php70-pecl-apc = %{apcu_bc_ver}
%description -n php70-pecl-apcu-bc
Provides the apc_* functions on top of APCu (extension name "apc").

%package -n php70-pecl-event
Summary:        libevent bindings (%{event_ver})
Version:        %{event_ver}
Requires:       php70 = 7.0.33
%description -n php70-pecl-event
Event - efficient scheduling of I/O, time and signal events (libevent).

%package -n php70-pecl-igbinary
Summary:        igbinary serializer (%{igbinary_ver})
Version:        %{igbinary_ver}
Requires:       php70 = 7.0.33
%description -n php70-pecl-igbinary
igbinary - a compact binary serializer.

%package -n php70-pecl-memcache
Summary:        memcache client (%{memcache_ver})
Version:        %{memcache_ver}
Requires:       php70 = 7.0.33
%description -n php70-pecl-memcache
memcache - memcached client (ext/memcache).

%package -n php70-pecl-memcached
Summary:        memcached client using libmemcached (%{memcached_ver})
Version:        %{memcached_ver}
Requires:       php70-pecl-igbinary = %{igbinary_ver}
Requires:       php70-pecl-msgpack = %{msgpack_ver}
%description -n php70-pecl-memcached
memcached - memcached client based on libmemcached, with igbinary,
msgpack, json and SASL support.

%package -n php70-pecl-mongodb
Summary:        MongoDB driver (%{mongodb_ver})
Version:        %{mongodb_ver}
Requires:       php70 = 7.0.33
%description -n php70-pecl-mongodb
MongoDB driver (ext/mongodb).

%package -n php70-pecl-msgpack
Summary:        MessagePack serializer (%{msgpack_ver})
Version:        %{msgpack_ver}
Requires:       php70 = 7.0.33
%description -n php70-pecl-msgpack
msgpack - MessagePack serializer.

%package -n php70-pecl-phpiredis
Summary:        hiredis bindings (%{phpiredis_ver})
Version:        %{phpiredis_ver}
Requires:       php70 = 7.0.33
%description -n php70-pecl-phpiredis
phpiredis - client extension for Redis based on hiredis.

%package -n php70-pecl-redis
Summary:        Redis client (%{redis_ver})
Version:        %{redis_ver}
Requires:       php70-pecl-igbinary = %{igbinary_ver}
Requires:       php70-pecl-msgpack = %{msgpack_ver}
%description -n php70-pecl-redis
phpredis - Redis client, with igbinary, msgpack and lzf support.

%package -n php70-pecl-xdebug
Summary:        Xdebug debugger and profiler (%{xdebug_ver})
Version:        %{xdebug_ver}
Requires:       php70 = 7.0.33
%description -n php70-pecl-xdebug
Xdebug. Not recommended on production servers (slows down every request).

# ----------------------------------------------------------------------
%prep
%setup -q -c -T
for s in %{SOURCE0} %{SOURCE1} %{SOURCE2} %{SOURCE3} %{SOURCE4} %{SOURCE5} \
         %{SOURCE6} %{SOURCE7} %{SOURCE8} %{SOURCE9} %{SOURCE10}; do
    tar xzf $s
done
rm -f package*.xml

# redis and memcached look for the serializer headers in ./ext/<name>/ first
for d in redis-%{redis_ver} memcached-%{memcached_ver}; do
    mkdir -p $d/ext/igbinary $d/ext/msgpack
    cp -a igbinary-%{igbinary_ver}/igbinary.h igbinary-%{igbinary_ver}/php_igbinary.h \
          igbinary-%{igbinary_ver}/src $d/ext/igbinary/
    cp -a msgpack-%{msgpack_ver}/*.h $d/ext/msgpack/
done

%build
build_ext() {
    dir=$1; shift
    pushd $dir
    phpize
    %configure --with-php-config=%{_bindir}/php-config "$@"
    make %{?_smp_mflags}
    popd
}

build_ext apcu-%{apcu_ver}            --enable-apcu
# apcu_bc includes "ext/apcu/*.h"
mkdir -p inc/ext/apcu && cp -a apcu-%{apcu_ver}/*.h inc/ext/apcu/
CPPFLAGS="-I$PWD/inc" build_ext apcu_bc-%{apcu_bc_ver} --enable-apc
build_ext event-%{event_ver}          --with-event-core --with-event-extra --with-event-openssl \
                                      --with-event-libevent-dir=%{_prefix} --disable-event-sockets
build_ext igbinary-%{igbinary_ver}    --enable-igbinary
build_ext memcache-%{memcache_ver}    --enable-memcache
build_ext msgpack-%{msgpack_ver}      --with-msgpack
build_ext memcached-%{memcached_ver}  --enable-memcached --enable-memcached-igbinary \
                                      --enable-memcached-msgpack --enable-memcached-json \
                                      --enable-memcached-session --enable-memcached-sasl \
                                      --with-libmemcached-dir=%{_prefix} --with-zlib-dir=%{_prefix}
build_ext mongodb-%{mongodb_ver}      --enable-mongodb --with-mongodb-ssl=openssl \
                                      --with-mongodb-sasl=cyrus
build_ext phpiredis-%{phpiredis_ver}  --enable-phpiredis --with-hiredis-dir=%{_prefix}
build_ext redis-%{redis_ver}          --enable-redis --enable-redis-igbinary \
                                      --enable-redis-msgpack --enable-redis-lzf
build_ext xdebug-%{xdebug_ver}        --enable-xdebug

%install
mkdir -p %{buildroot}%{php_extdir} %{buildroot}%{php_inidir}
for m in apcu-%{apcu_ver}/modules/apcu.so apcu_bc-%{apcu_bc_ver}/modules/apc.so \
         event-%{event_ver}/modules/event.so igbinary-%{igbinary_ver}/modules/igbinary.so \
         memcache-%{memcache_ver}/modules/memcache.so memcached-%{memcached_ver}/modules/memcached.so \
         mongodb-%{mongodb_ver}/modules/mongodb.so msgpack-%{msgpack_ver}/modules/msgpack.so \
         phpiredis-%{phpiredis_ver}/modules/phpiredis.so redis-%{redis_ver}/modules/redis.so \
         xdebug-%{xdebug_ver}/modules/xdebug.so; do
    install -m 755 $m %{buildroot}%{php_extdir}/
done

# Load order: serializers and apcu before the modules that depend on them.
# event depends on sockets, which is built into php70.
ini() { printf '%s\n' "$2" > %{buildroot}%{php_inidir}/$1; }
ini 40-apcu.ini      'extension=apcu.so'
ini 50-apc.ini       'extension=apc.so'
ini 40-igbinary.ini  'extension=igbinary.so'
ini 40-msgpack.ini   'extension=msgpack.so'
ini 40-memcache.ini  'extension=memcache.so'
ini 50-memcached.ini 'extension=memcached.so'
ini 40-mongodb.ini   'extension=mongodb.so'
ini 40-phpiredis.ini 'extension=phpiredis.so'
ini 50-redis.ini     'extension=redis.so'
ini 40-event.ini     'extension=event.so'
ini 15-xdebug.ini    'zend_extension=xdebug.so'

%check
# Each module must load together with all the others
args=""
for i in 40-apcu 50-apc 40-igbinary 40-msgpack 40-memcache 50-memcached \
         40-mongodb 40-phpiredis 50-redis 40-event; do
    args="$args -d $(cat %{buildroot}%{php_inidir}/$i.ini)"
done
php -n -d extension_dir=%{buildroot}%{php_extdir} $args \
    -d zend_extension=%{buildroot}%{php_extdir}/xdebug.so -m > modules.txt
cat modules.txt
for m in apcu apc event igbinary memcache memcached mongodb msgpack phpiredis redis xdebug; do
    grep -qx "$m" modules.txt
done

%files -n php70-pecl-apcu
%{php_extdir}/apcu.so
%config(noreplace) %{php_inidir}/40-apcu.ini

%files -n php70-pecl-apcu-bc
%{php_extdir}/apc.so
%config(noreplace) %{php_inidir}/50-apc.ini

%files -n php70-pecl-event
%{php_extdir}/event.so
%config(noreplace) %{php_inidir}/40-event.ini

%files -n php70-pecl-igbinary
%{php_extdir}/igbinary.so
%config(noreplace) %{php_inidir}/40-igbinary.ini

%files -n php70-pecl-memcache
%{php_extdir}/memcache.so
%config(noreplace) %{php_inidir}/40-memcache.ini

%files -n php70-pecl-memcached
%{php_extdir}/memcached.so
%config(noreplace) %{php_inidir}/50-memcached.ini

%files -n php70-pecl-mongodb
%{php_extdir}/mongodb.so
%config(noreplace) %{php_inidir}/40-mongodb.ini

%files -n php70-pecl-msgpack
%{php_extdir}/msgpack.so
%config(noreplace) %{php_inidir}/40-msgpack.ini

%files -n php70-pecl-phpiredis
%{php_extdir}/phpiredis.so
%config(noreplace) %{php_inidir}/40-phpiredis.ini

%files -n php70-pecl-redis
%{php_extdir}/redis.so
%config(noreplace) %{php_inidir}/50-redis.ini

%files -n php70-pecl-xdebug
%{php_extdir}/xdebug.so
%config(noreplace) %{php_inidir}/15-xdebug.ini

%changelog
* Wed Oct 21 2026 Umut KIRGOZ <umut.kirgoz@mynetgroup.com> - 7.0.33-1.sec1
- Initial package for php70 7.0.33-1.sec1
