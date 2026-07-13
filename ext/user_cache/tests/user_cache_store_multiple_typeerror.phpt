--TEST--
UserCache\Cache: storeMultiple() rejects unstorable values and rolls back
--EXTENSIONS--
user_cache
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
$cache = UserCache\Cache::getPool('store-multiple-typeerror');

var_dump($cache->store('existing', 'previous'));

try {
    $cache->storeMultiple([
        'existing' => 'replaced',
        'fresh' => 1,
        'bad' => fn() => 1,
    ]);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
var_dump($cache->fetch('existing'));
var_dump($cache->has('fresh'));

try {
    $cache->storeMultiple(['bad' => fopen('php://memory', 'r')]);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
bool(true)
UserCache\Cache::storeMultiple(): Argument #1 ($values) must not contain Closure objects
string(8) "previous"
bool(false)
UserCache\Cache::storeMultiple(): Argument #1 ($values) must contain only values of type object|array|string|int|float|bool|null, resource given
