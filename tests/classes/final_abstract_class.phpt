--TEST--
An abstract class can be final.
--FILE--
<?php

final abstract class Cache
{
    private static array $cache = [];

    public static function set(string $k, mixed $v): void
    {
        static::$cache[$k] = $v;
    }

    public static function get(string $k): mixed
    {
        return static::$cache[$k] ?? null;
    }
}

Cache::set('value', "OK\n");

echo Cache::get('value');
?>
--EXPECTF--
OK
