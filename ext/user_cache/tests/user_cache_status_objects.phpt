--TEST--
UserCache\Cache: status objects
--EXTENSIONS--
user_cache
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php

$cache = UserCache\Cache::getPool('status-objects');
$status = UserCache\Cache::getStatus();
$poolStatus = $cache->getPoolStatus();

var_dump($status instanceof UserCache\CacheStatus);
var_dump($poolStatus instanceof UserCache\CachePoolStatus);
var_dump($status->getAvailability() === UserCache\CacheAvailability::Available);
var_dump($poolStatus->getPoolName());
var_dump($poolStatus->getEntryCount());
var_dump($poolStatus->getEntryKeys());
var_dump($poolStatus->getUsedMemory());

var_dump($cache->store('a', 1));
var_dump($cache->store('b', 2));

$other = UserCache\Cache::getPool('status-objects-other');
var_dump($other->store('a', 3));

$status = UserCache\Cache::getStatus();
$poolStatus = $cache->getPoolStatus();
$otherStatus = $other->getPoolStatus();

var_dump($status->getEntryCount() >= 3);
var_dump($poolStatus->getEntryCount());
var_dump($otherStatus->getEntryCount());
$keys = $poolStatus->getEntryKeys();
sort($keys);
var_dump($keys);
var_dump($poolStatus->getUsedMemory() > 0);

foreach ([$status, $poolStatus] as $object) {
    try {
        serialize($object);
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
}

try {
    $status->entryCount = 0;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECTF--
bool(true)
bool(true)
bool(true)
string(14) "status-objects"
int(0)
array(0) {
}
int(0)
bool(true)
bool(true)
bool(true)
bool(true)
int(2)
int(1)
array(2) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "b"
}
bool(true)
Exception: Serialization of 'UserCache\CacheStatus' is not allowed
Exception: Serialization of 'UserCache\CachePoolStatus' is not allowed
Error: Cannot create dynamic property UserCache\CacheStatus::$entryCount
