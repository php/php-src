--TEST--
UserCache\Cache: remember() passes the cache key to the callback
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
$cache = UserCache\Cache::getPool('remember-key');

$loader = static function (string $key): string {
    return 'loaded:' . $key;
};

var_dump($cache->remember('first', $loader));
var_dump($cache->remember('second', $loader));

$calls = 0;
$counting = static function (string $key) use (&$calls): string {
    $calls++;
    return 'value:' . $key;
};
var_dump($cache->remember('first', $counting));
var_dump($calls);

var_dump($cache->remember('third', static fn (): int => 42));
?>
--EXPECT--
string(12) "loaded:first"
string(13) "loaded:second"
string(12) "loaded:first"
int(0)
int(42)
