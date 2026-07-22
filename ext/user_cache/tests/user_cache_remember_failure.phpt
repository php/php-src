--TEST--
UserCache\Cache: remember() failure paths (callback throw, unstorable result)
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
$cache = UserCache\Cache::getPool('remember-failure');

/* Exceptions must not leave the entry lock held. */
try {
    $cache->remember('boom', function () {
        throw new RuntimeException('from callback');
    });
} catch (RuntimeException $e) {
    echo $e->getMessage(), "\n";
}
var_dump($cache->has('boom'));
var_dump($cache->lock('boom'));
var_dump($cache->unlock('boom'));

foreach ([
    'res' => fn() => fopen('php://memory', 'r'),
    'closure' => fn() => fn() => 1,
    'nested-res' => fn() => ['inner' => fopen('php://memory', 'r')],
] as $key => $callback) {
    try {
        $cache->remember($key, $callback);
    } catch (TypeError $e) {
        echo $e->getMessage(), "\n";
    }
    var_dump($cache->has($key));
}
?>
--EXPECT--
from callback
bool(false)
bool(true)
bool(true)
resources cannot be stored in the user cache
bool(false)
Closure objects cannot be stored in the user cache
bool(false)
resources cannot be stored in the user cache
bool(false)
