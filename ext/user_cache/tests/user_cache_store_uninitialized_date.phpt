--TEST--
UserCache\Cache: storing an uninitialized date object throws DateObjectError instead of returning false
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
date.timezone=UTC
--FILE--
<?php
$cache = UserCache\Cache::getPool('uninitialized-date');

$classes = [
    DateTime::class,
    DateTimeZone::class,
    DateInterval::class,
    DatePeriod::class,
];

/* The safe-direct handlers decline these objects; declining without an
 * exception used to surface as a plain false, which is indistinguishable
 * from an environment failure. Native serialize() throws here too. */
foreach ($classes as $class) {
    $object = (new ReflectionClass($class))->newInstanceWithoutConstructor();

    try {
        $result = $cache->store('date', $object);

        echo $class, ': stored, returned ', var_export($result, true), "\n";
    } catch (Error $e) {
        echo $class, ': ', get_class($e), ', message ',
            str_contains($e->getMessage(), 'has not been correctly initialized') ? 'OK' : $e->getMessage(),
            "\n";
    }
}

/* No entry was written and the pool is still usable afterwards. */
var_dump($cache->has('date'));
var_dump($cache->store('scalar', 7));
var_dump($cache->fetch('scalar'));
?>
--EXPECT--
DateTime: DateObjectError, message OK
DateTimeZone: DateObjectError, message OK
DateInterval: DateObjectError, message OK
DatePeriod: DateObjectError, message OK
bool(false)
bool(true)
int(7)
