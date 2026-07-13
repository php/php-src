--TEST--
UserCache\Cache: disabled by INI size
--EXTENSIONS--
user_cache
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=0
--FILE--
<?php
$cache = UserCache\Cache::getPool('off');
$status = UserCache\Cache::getStatus();
$poolStatus = $cache->getPoolStatus();
var_dump($poolStatus->getPoolName(), $status->getAvailability()->name, $poolStatus->getEntryCount(), $poolStatus->getEntryKeys(), $poolStatus->getUsedMemory());
var_dump($status->getConfiguredMemory());
var_dump($cache->store('key', 1));
var_dump($cache->add('key', 1));
var_dump($cache->storeMultiple(['key' => 1]));
var_dump($cache->increment('key'));
var_dump($cache->decrement('key'));
var_dump($cache->has('key'));
var_dump($cache->fetch('key', 'default'));
var_dump($cache->fetchMultiple(['key', 'other'], 'default'));
var_dump($cache->delete('key'));
var_dump($cache->deleteMultiple(['key', 'other']));
var_dump($cache->clear());
var_dump($cache->lock('key'));
var_dump($cache->unlock('key'));
var_dump($cache->remember('key', fn() => 42));

var_dump($status->getAvailability());
?>
--EXPECT--
string(3) "off"
string(13) "DisabledByIni"
int(0)
array(0) {
}
int(0)
int(0)
bool(false)
bool(false)
bool(false)
NULL
NULL
bool(false)
string(7) "default"
array(2) {
  ["key"]=>
  string(7) "default"
  ["other"]=>
  string(7) "default"
}
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
int(42)
enum(UserCache\CacheAvailability::DisabledByIni)
