--TEST--
UserCache\Cache: atomic increment and decrement, and error messages that cut a key at its NUL byte while the full key is stored and updated
--INI--
user_cache.enable=1
user_cache.enable_cli=1
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
function nul_key_messages(): void
{
    $cache = UserCache\Cache::getPool('atomic-nul-key');
    $cache->clear();

    var_dump($cache->store('text', 5), $cache->store("text\0tail", 'x'));
    try {
        $cache->increment("text\0tail");
    } catch (\ValueError $e) {
        echo $e->getMessage(), "\n";
    }
    try {
        $cache->decrement("text\0tail");
    } catch (\ValueError $e) {
        echo $e->getMessage(), "\n";
    }

    var_dump($cache->store("max\0tail", PHP_INT_MAX), $cache->store("min\0tail", PHP_INT_MIN));
    try {
        $cache->increment("max\0tail");
    } catch (\ArithmeticError $e) {
        echo $e->getMessage(), "\n";
    }
    try {
        $cache->decrement("min\0tail");
    } catch (\ArithmeticError $e) {
        echo $e->getMessage(), "\n";
    }

    var_dump($cache->increment("n\0a", 2), $cache->increment("n\0b", 3), $cache->decrement("n\0a"));
    var_dump($cache->fetch('text'), $cache->fetch("text\0tail"), $cache->fetch('n', 'MISS'), $cache->fetch("n\0a"), $cache->fetch("n\0b"));
    var_dump($cache->fetch("max\0tail") === PHP_INT_MAX, $cache->fetch("min\0tail") === PHP_INT_MIN);

    $keys = str_replace("\0", '\0', $cache->getPoolStatus()->getEntryKeys());
    sort($keys);
    echo implode(',', $keys), "\n";
}

echo "\nNUL byte in the key:\n";
nul_key_messages();
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

NUL byte in the key:
bool(true)
bool(true)
Increment of user cache key "text" requires the stored value to be an integer
Decrement of user cache key "text" requires the stored value to be an integer
bool(true)
bool(true)
Increment of user cache key "max" would exceed the range of a PHP integer
Decrement of user cache key "min" would exceed the range of a PHP integer
int(2)
int(3)
int(1)
int(5)
string(1) "x"
string(4) "MISS"
int(1)
int(3)
bool(true)
bool(true)
max\0tail,min\0tail,n\0a,n\0b,text,text\0tail
