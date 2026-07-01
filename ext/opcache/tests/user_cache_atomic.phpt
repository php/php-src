--TEST--
OPcache User Cache: atomic increment and decrement
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_cache_only=0
opcache.user_cache_shm_size=16M
--FILE--
<?php
$cache = new Opcache\UserCache('atomic');
$cache->clear();

var_dump($cache->increment('counter'));
var_dump($cache->increment('counter', 4));
var_dump($cache->decrement('counter', 2));
var_dump($cache->fetch('counter'));

var_dump($cache->decrement('missing', 3));
var_dump($cache->fetch('missing'));

var_dump($cache->store('string', 'x'));
var_dump($cache->increment('string'));
var_dump($cache->fetch('string'));

var_dump($cache->store('max', PHP_INT_MAX));
var_dump($cache->increment('max'));
var_dump($cache->fetch('max') === PHP_INT_MAX);

var_dump($cache->store('min', PHP_INT_MIN));
var_dump($cache->decrement('min'));
var_dump($cache->fetch('min') === PHP_INT_MIN);

try {
    $cache->increment('counter', -1);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

try {
    $cache->decrement('counter', -1);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECTF--
int(1)
int(5)
int(3)
int(3)
int(-3)
int(-3)
bool(true)
bool(false)
string(1) "x"
bool(true)
bool(false)
bool(true)
bool(true)
bool(false)
bool(true)
Opcache\UserCache::increment(): Argument #2 ($step) must be greater than or equal to 0
Opcache\UserCache::decrement(): Argument #2 ($step) must be greater than or equal to 0
