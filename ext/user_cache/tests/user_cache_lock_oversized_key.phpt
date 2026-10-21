--TEST--
UserCache\Cache: a lock key that cannot fit in the segment fails without clearing the cache
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=64K
user_cache.eviction_policy=lru
--FILE--
<?php
$cache = UserCache\Cache::getPool('lock-oversized-key');
$cache->clear();
var_dump($cache->store('kept', 'value'));
$expunged = UserCache\Cache::getStatus()->getExpungeCount();

var_dump($cache->lock(str_repeat('k', 40000)));
var_dump($cache->remember(str_repeat('r', 40000), fn () => 'computed without a lock'));
var_dump($cache->fetch('kept'), UserCache\Cache::getStatus()->getExpungeCount() === $expunged);
?>
--EXPECT--
bool(true)
bool(false)
string(23) "computed without a lock"
string(5) "value"
bool(true)
