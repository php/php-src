--TEST--
UserCache\Cache: remember() keeps a lock the caller already holds
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
$cache = UserCache\Cache::getPool('remember-preheld');

var_dump($cache->lock('key'));
var_dump($cache->remember('key', fn () => 'value'));
var_dump($cache->fetch('key'));
var_dump($cache->unlock('key'));

var_dump($cache->lock('throwing'));
try {
    $cache->remember('throwing', function () {
        throw new RuntimeException('boom');
    });
} catch (RuntimeException $e) {
    echo $e->getMessage(), "\n";
}
var_dump($cache->unlock('throwing'));

var_dump($cache->lock('unstorable'));
try {
    $cache->remember('unstorable', fn () => fn () => 1);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
var_dump($cache->unlock('unstorable'));

var_dump($cache->remember('own', fn () => 'own-value'));
var_dump($cache->unlock('own'));
?>
--EXPECT--
bool(true)
string(5) "value"
string(5) "value"
bool(true)
bool(true)
boom
bool(true)
bool(true)
Closure objects cannot be stored in the user cache
bool(true)
string(9) "own-value"
bool(false)
