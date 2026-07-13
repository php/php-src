--TEST--
UserCache\Cache: atomic increment and decrement
--EXTENSIONS--
user_cache
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
$cache = UserCache\Cache::getPool('atomic');
$cache->clear();

var_dump($cache->increment('counter'));
var_dump($cache->increment('counter', 4));
var_dump($cache->decrement('counter', 2));
var_dump($cache->fetch('counter'));

var_dump($cache->decrement('missing', 3));
var_dump($cache->fetch('missing'));

var_dump($cache->store('string', 'x'));
try {
    $cache->increment('string');
} catch (\ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    $cache->decrement('string');
} catch (\ValueError $e) {
    echo $e->getMessage(), "\n";
}
var_dump($cache->fetch('string'));

var_dump($cache->store('max', PHP_INT_MAX));
try {
    $cache->increment('max');
} catch (\ArithmeticError $e) {
    echo $e->getMessage(), "\n";
}
var_dump($cache->fetch('max') === PHP_INT_MAX);

var_dump($cache->store('min', PHP_INT_MIN));
try {
    $cache->decrement('min');
} catch (\ArithmeticError $e) {
    echo $e->getMessage(), "\n";
}
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
Increment of user cache key "string" requires the stored value to be an integer
Decrement of user cache key "string" requires the stored value to be an integer
string(1) "x"
bool(true)
Increment of user cache key "max" would exceed the range of a PHP integer
bool(true)
bool(true)
Decrement of user cache key "min" would exceed the range of a PHP integer
bool(true)
UserCache\Cache::increment(): Argument #2 ($step) must be greater than or equal to 0
UserCache\Cache::decrement(): Argument #2 ($step) must be greater than or equal to 0
