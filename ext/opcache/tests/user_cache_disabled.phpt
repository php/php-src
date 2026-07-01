--TEST--
OPcache User Cache: disabled by INI size
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_cache_only=0
opcache.user_cache_shm_size=0
--FILE--
<?php
$cache = new Opcache\UserCache('off');
$info = $cache->info();
var_dump($info->scope, $info->available, $info->unavailableReason);
var_dump($info->enabled, $info->backendInitialized, $info->configuredMemory);
var_dump($cache->store('key', 1));
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
?>
--EXPECT--
string(3) "off"
bool(false)
string(63) "OPcache User Cache is disabled by opcache.user_cache_shm_size=0"
bool(false)
bool(false)
int(0)
bool(false)
bool(false)
bool(false)
bool(false)
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
