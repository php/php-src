--TEST--
UserCache\Cache: lazy objects and not-serializable internal objects are rejected
--EXTENSIONS--
user_cache
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
$cache = UserCache\Cache::getPool('unstorable-extended');

class LazyTarget
{
    public int $x = 1;
}

$reflector = new ReflectionClass(LazyTarget::class);

$values = [
    'ghost' => $reflector->newLazyGhost(function ($object) { $object->x = 2; }),
    'proxy' => $reflector->newLazyProxy(fn() => new LazyTarget()),
    'nested-ghost' => ['inner' => $reflector->newLazyGhost(function ($object) { $object->x = 2; })],
    'not-serializable' => UserCache\Cache::getStatus(),
    'weakref' => WeakReference::create(new LazyTarget()),
    'nested-weakref' => ['inner' => WeakReference::create(new LazyTarget())],
];

foreach ($values as $key => $value) {
    try {
        $cache->store($key, $value);
    } catch (TypeError $e) {
        echo $e->getMessage(), "\n";
    }
    var_dump($cache->has($key));
}
?>
--EXPECT--
lazy objects cannot be stored in the user cache
bool(false)
lazy objects cannot be stored in the user cache
bool(false)
lazy objects cannot be stored in the user cache
bool(false)
objects with opaque internal state (e.g. Fiber, Generator, PDO) cannot be stored in the user cache
bool(false)
objects with opaque internal state (e.g. Fiber, Generator, PDO) cannot be stored in the user cache
bool(false)
objects with opaque internal state (e.g. Fiber, Generator, PDO) cannot be stored in the user cache
bool(false)
