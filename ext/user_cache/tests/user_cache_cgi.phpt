--TEST--
CGI: UserCache\Cache is available
--CGI--
--INI--
user_cache.enable=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--ENV--
DOCUMENT_ROOT=/tmp/php-user-cache-cgi
SERVER_NAME=php-user-user_cache.local
--FILE--
<?php
$cache = UserCache\Cache::getPool('cgi');
$cache->clear();
$status = UserCache\Cache::getStatus();
$poolStatus = $cache->getPoolStatus();

var_dump(php_sapi_name());
var_dump($status->getAvailability()->name, $poolStatus->getEntryCount());
var_dump($cache->store('key', 'value'));
var_dump($cache->fetch('key', 'MISS'));
?>
--EXPECT--
string(8) "cgi-fcgi"
string(9) "Available"
int(0)
bool(true)
string(5) "value"
