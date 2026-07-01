--TEST--
CGI: OPcache User Cache is unavailable without a cache boundary
--CGI--
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.file_cache_only=0
opcache.user_cache_shm_size=16M
--ENV--
DOCUMENT_ROOT=
SERVER_NAME=
--FILE--
<?php
$cache = new Opcache\UserCache('cgi-no-boundary');
$info = $cache->info();

var_dump($info->available);
var_dump($cache->store('key', 'value'));
var_dump($cache->fetch('key', 'MISS'));
var_dump($cache->delete('key'));
?>
--EXPECT--
bool(false)
bool(false)
string(4) "MISS"
bool(true)
