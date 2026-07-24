--TEST--
UserCache\Cache: negative TTL throws ValueError on every write API
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
$cache = UserCache\Cache::getPool('negative-ttl');

$calls = [
    'store' => fn() => $cache->store('key', 1, -1),
    'add' => fn() => $cache->add('key', 1, -1),
    'storeMultiple' => fn() => $cache->storeMultiple(['key' => 1], -1),
    'increment' => fn() => $cache->increment('key', 1, -1),
    'decrement' => fn() => $cache->decrement('key', 1, -1),
    'remember' => fn() => $cache->remember('key', fn() => 1, -1),
];

foreach ($calls as $fn) {
    try {
        $fn();
    } catch (ValueError $e) {
        echo $e->getMessage(), "\n";
    }
}

var_dump($cache->has('key'));
?>
--EXPECT--
UserCache\Cache::store(): Argument #3 ($ttl) must be greater than or equal to 0
UserCache\Cache::add(): Argument #3 ($ttl) must be greater than or equal to 0
UserCache\Cache::storeMultiple(): Argument #2 ($ttl) must be greater than or equal to 0
UserCache\Cache::increment(): Argument #3 ($ttl) must be greater than or equal to 0
UserCache\Cache::decrement(): Argument #3 ($ttl) must be greater than or equal to 0
UserCache\Cache::remember(): Argument #3 ($ttl) must be greater than or equal to 0
bool(false)
