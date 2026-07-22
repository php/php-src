--TEST--
UserCache\Cache: expunge and store-failure counters are exposed
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=2M
--FILE--
<?php
$cache = UserCache\Cache::getPool('counters');
$cache->clear();

$info = UserCache\Cache::getStatus();
$expungeBase = $info->getExpungeCount();
$storeFailureBase = $info->getStoreFailureCount();

/* Fill until pressure resets the partition. */
$payload = str_repeat('x', 64 * 1024);
for ($i = 0; $i < 64; $i++) {
    $cache->store('big-' . $i, $payload . $i);
}

$info = UserCache\Cache::getStatus();
var_dump($info->getExpungeCount() > $expungeBase);

var_dump($cache->store('too-big', str_repeat('y', 4 * 1024 * 1024)));
$info = UserCache\Cache::getStatus();
var_dump($info->getStoreFailureCount() > $storeFailureBase);
?>
--EXPECT--
bool(true)
bool(false)
bool(true)
