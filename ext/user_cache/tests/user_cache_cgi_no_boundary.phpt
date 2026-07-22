--TEST--
CGI: UserCache\Cache is unavailable without a cache boundary
--CGI--
--INI--
user_cache.enable=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--ENV--
DOCUMENT_ROOT=
SERVER_NAME=
--FILE--
<?php
$cache = UserCache\Cache::getPool('cgi-no-boundary');
$status = UserCache\Cache::getStatus();
$poolStatus = $cache->getPoolStatus();

var_dump($status->getAvailability()->name, $poolStatus->getEntryCount(), $poolStatus->getEntryKeys(), $poolStatus->getUsedMemory());
var_dump($cache->store('key', 'value'));
var_dump($cache->fetch('key', 'MISS'));
var_dump($cache->delete('key'));
?>
--EXPECT--
string(31) "UnavailableByCgiFastCgiBoundary"
int(0)
array(0) {
}
int(0)
bool(false)
string(4) "MISS"
bool(true)
