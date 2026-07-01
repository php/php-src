--TEST--
CGI: OPcache User Cache is available
--CGI--
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.file_cache_only=0
opcache.user_cache_shm_size=16M
--ENV--
DOCUMENT_ROOT=/tmp/php-user-cache-cgi
SERVER_NAME=php-user-cache.local
--FILE--
<?php
$cache = new Opcache\UserCache('cgi');
$info = $cache->info();

var_dump(php_sapi_name());
var_dump($info->available, $info->unavailableReason);
var_dump($cache->store('key', 'value'));
var_dump($cache->fetch('key', 'MISS'));
?>
--EXPECT--
string(8) "cgi-fcgi"
bool(true)
NULL
bool(true)
string(5) "value"
