--TEST--
UserCache\Cache: increment() and decrement() under the global lock (TTL, expiring entry, held entry lock) keep range and type checks
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=16M
--FILE--
<?php
function attempt(callable $operation): string
{
    try {
        return var_export($operation(), true);
    } catch (Error $e) {
        return get_class($e) . ': ' . $e->getMessage();
    }
}

$cache = UserCache\Cache::getPool('atomic-locked');
$cache->clear();

/* A TTL argument never takes the lock-free scalar path */
echo attempt(fn() => $cache->increment('ttl-arg', 5, 60)), "\n";
echo attempt(fn() => $cache->decrement('ttl-arg', 7, 60)), "\n";
echo attempt(fn() => $cache->increment('ttl-arg', 3, 60)), "\n";
var_dump($cache->fetch('ttl-arg'));
echo attempt(fn() => $cache->decrement('ttl-missing', 4, 60)), "\n";
var_dump($cache->fetch('ttl-missing'));

/* Entries that carry a TTL are updated under the lock even without a TTL argument */
var_dump($cache->store('expiring-max', PHP_INT_MAX - 1, 60));
echo attempt(fn() => $cache->increment('expiring-max')), "\n";
echo attempt(fn() => $cache->increment('expiring-max')), "\n";
var_dump($cache->fetch('expiring-max') === PHP_INT_MAX);

var_dump($cache->store('expiring-min', PHP_INT_MIN + 1, 60));
echo attempt(fn() => $cache->decrement('expiring-min')), "\n";
echo attempt(fn() => $cache->decrement('expiring-min')), "\n";
echo attempt(fn() => $cache->decrement('expiring-min', 0)), "\n";
var_dump($cache->fetch('expiring-min') === PHP_INT_MIN);

var_dump($cache->store('expiring-text', 'text', 60));
var_dump($cache->store('expiring-float', 1.5, 60));
echo attempt(fn() => $cache->increment('expiring-text')), "\n";
echo attempt(fn() => $cache->decrement('expiring-text')), "\n";
echo attempt(fn() => $cache->decrement('expiring-float', 1, 60)), "\n";
var_dump($cache->fetch('expiring-text'), $cache->fetch('expiring-float'));

/* A key held by lock() is updated under the lock and stays coherent afterwards */
var_dump($cache->store('held', 10));
var_dump($cache->lock('held'));
echo attempt(fn() => $cache->decrement('held', 20)), "\n";
echo attempt(fn() => $cache->increment('held', PHP_INT_MAX)), "\n";
echo attempt(fn() => $cache->increment('held', 11)), "\n";
var_dump($cache->fetch('held') === PHP_INT_MAX - 10);
var_dump($cache->lock('held-missing'));
echo attempt(fn() => $cache->decrement('held-missing', 4)), "\n";
var_dump($cache->unlock('held'), $cache->unlock('held-missing'));

echo attempt(fn() => $cache->increment('held')), "\n";
var_dump($cache->fetch('held') === PHP_INT_MAX - 9);
var_dump($cache->fetch('held-missing'));
?>
--EXPECTF--
5
-2
1
int(1)
-4
int(-4)
bool(true)
%d
ArithmeticError: Increment of user cache key "expiring-max" would exceed the range of a PHP integer
bool(true)
bool(true)
-%d-1
ArithmeticError: Decrement of user cache key "expiring-min" would exceed the range of a PHP integer
-%d-1
bool(true)
bool(true)
bool(true)
ValueError: Increment of user cache key "expiring-text" requires the stored value to be an integer
ValueError: Decrement of user cache key "expiring-text" requires the stored value to be an integer
ValueError: Decrement of user cache key "expiring-float" requires the stored value to be an integer
string(4) "text"
float(1.5)
bool(true)
bool(true)
-10
%d
ArithmeticError: Increment of user cache key "held" would exceed the range of a PHP integer
bool(true)
bool(true)
-4
bool(true)
bool(true)
%d
bool(true)
int(-4)
