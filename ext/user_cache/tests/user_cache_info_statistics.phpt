--TEST--
UserCache\Cache: info statistics
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
$cache = UserCache\Cache::getPool('info-stats-a');
$other = UserCache\Cache::getPool('info-stats-b');
$cache->clear();
$other->clear();

$initial = UserCache\Cache::getStatus();
$initialPool = $cache->getPoolStatus();
var_dump($initialPool->getPoolName());
var_dump($initial->getAvailability()->name);
var_dump($initial->getConfiguredMemory() === 16 * 1024 * 1024);
var_dump($initial->getSharedMemorySize() > 0);
var_dump($initial->getEntryCount());
var_dump($initialPool->getEntryCount());

var_dump($cache->store('key', ['value' => 1]));
var_dump($other->store('key', 'other'));

$afterStore = UserCache\Cache::getStatus();
$afterStorePool = $cache->getPoolStatus();
$afterStoreOther = $other->getPoolStatus();
var_dump($afterStore->getEntryCount() >= 2);
var_dump($afterStorePool->getEntryCount());
var_dump($afterStoreOther->getEntryCount());
var_dump($afterStore->getEntryCapacity() > 0);
var_dump($afterStore->getUsedMemory() > 0);
var_dump($afterStore->getFreeMemory() > 0);
var_dump($afterStore->getWastedMemory() >= 0);
var_dump($afterStore->getTombstoneCount() >= 0);
var_dump($afterStorePool->getUsedMemory() > 0);

var_dump($cache->clear());
var_dump($other->clear());
var_dump($cache->fetch('key', 'missing'));
var_dump($other->fetch('key', 'missing'));

$afterClear = UserCache\Cache::getStatus();
$afterClearPool = $cache->getPoolStatus();
var_dump($afterClear->getEntryCount());
var_dump($afterClearPool->getEntryCount());
?>
--EXPECT--
string(12) "info-stats-a"
string(9) "Available"
bool(true)
bool(true)
int(0)
int(0)
bool(true)
bool(true)
bool(true)
int(1)
int(1)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
string(7) "missing"
string(7) "missing"
int(0)
int(0)
