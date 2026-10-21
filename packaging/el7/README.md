# php70 7.0.33 (security backports) – CentOS 7 RPMs

PHP 7.0.33 + backport of GHSA-w8hw-w9fx-j79w (CVE-2026-109618) and
GHSA-6wmf-4rv5-p3rp (CVE-2026-109619), see the commit
"Avoid state corruption via bailout (backport to 7.0.33)".

## Build

    ./build.sh            # x86_64 (on Apple Silicon: emulated, slow)
    ./build.sh aarch64

The php source is taken from the committed `HEAD` (`git archive`),
uncommitted changes are not included. RPMs end up in `out/<arch>/`.

## Packages

| package | contents |
|---|---|
| php70 | `/usr/bin/php`, `libphp7.so` (mod_php, NTS, prefork), bundled extensions, opcache |
| php70-devel | phpize, php-config, headers |
| php70-pecl-* | apcu, apcu-bc (`apc`), event, igbinary, memcache, memcached, mongodb, msgpack, phpiredis, redis, xdebug |

Layout: `/etc/php.ini`, `/etc/php.d/*.ini`, `/usr/lib64/php/modules`,
`/etc/httpd/conf.modules.d/10-php.conf`, `/etc/httpd/conf.d/php.conf`,
sessions in `/var/lib/php/session`.

## Behavior change

A memory_limit error (and allocation size overflow) no longer longjmp()s:
the error is logged, then the process exits (`_exit(255)`). With mod_php the
Apache child exits and is replaced; the client sees the connection closed.
Shutdown functions and destructors do not run after such an error.
