--TEST--
UserCache\Cache: LRU eviction finds victims in a sparsely populated entry table
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=32M
--FILE--
<?php
$cache = UserCache\Cache::getPool('lru-sparse');
$status = static fn (): UserCache\CacheStatus => UserCache\Cache::getStatus();
$blob = str_repeat('x', 60000);

/* Large values fill the data region while occupying a few percent of the
 * entry table; sequential keys hash into one narrow band of slots. */
$stored = 0;
while ($status()->getEvictionCount() === 0 && $status()->getExpungeCount() === 0 && $stored < 100000) {
    var_dump($cache->store(sprintf('k%06d', $stored), $blob));
    $stored++;
}
var_dump($stored > 100);
var_dump($status()->getEvictionCount() > 0);
var_dump($status()->getExpungeCount() === 0);
var_dump($status()->getEntryCount() >= $stored - 2);

for ($i = 0; $i < 100; $i++) {
    $cache->store(sprintf('n%06d', $i), $blob);
}
var_dump($status()->getExpungeCount() === 0);
var_dump($status()->getEntryCount() >= $stored - 2);
?>
--EXPECTF--
%A
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
