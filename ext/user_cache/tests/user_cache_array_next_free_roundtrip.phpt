--TEST--
UserCache\Cache: nNextFreeElement round-trips exactly, including negative and out-of-uint32 values
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
$cache = UserCache\Cache::getPool('array-next-free');

function ok(string $label, bool $cond): void
{
    echo $label, ': ', $cond ? 'OK' : 'FAIL', "\n";
}

/* Keys are compared against a control copy that never entered the cache, so
 * the expectation stays valid whatever the engine's append rules are. */

/* (a) All-negative keys holding an object: the object makes the array
 * ineligible for the verbatim copy, forcing the position-independent
 * (dynamic array) encoding where next-free used to be truncated to 32 bits. */
$negativeObject = [-5 => new stdClass()];
$control = $negativeObject;
$cache->store('negative-object', $negativeObject);
$fetched = $cache->fetch('negative-object');
ok('negative object survived', $fetched[-5] instanceof stdClass);
$control[] = 'y';
$fetched[] = 'y';
ok('negative object control keys', array_keys($control) === [-5, -4]);
ok('negative object fetched keys', array_keys($fetched) === array_keys($control));

/* (b) Emptied array whose next-free index stayed negative: no elements at all,
 * so only the next-free field carries state across the cache. */
$emptied = [];
$emptied[-9] = 1;
unset($emptied[-9]);
$control = $emptied;
$cache->store('emptied-negative', $emptied);
$fetched = $cache->fetch('emptied-negative');
ok('emptied is empty', $fetched === []);
$control[] = 'x';
$fetched[] = 'x';
ok('emptied control keys', array_keys($control) === [-8]);
ok('emptied fetched keys', array_keys($fetched) === array_keys($control));

/* (c) Next-free above UINT32_MAX (snowflake-style ids). Keys are printed
 * relative to PHP_INT_MAX so the expectation is integer-width neutral. */
$hugeObject = [PHP_INT_MAX - 1 => new stdClass()];
$control = $hugeObject;
$cache->store('huge-object', $hugeObject);
$fetched = $cache->fetch('huge-object');
ok('huge object survived', $fetched[PHP_INT_MAX - 1] instanceof stdClass);
$control[] = 'z';
$fetched[] = 'z';
$offsets = static fn (array $array): string => implode(
    ',',
    array_map(static fn (int $key): int => $key - PHP_INT_MAX, array_keys($array))
);
echo 'huge control keys: ', $offsets($control), "\n";
echo 'huge fetched keys: ', $offsets($fetched), "\n";
ok('huge fetched keys match control', array_keys($fetched) === array_keys($control));

/* (d) Plain data with a negative key: eligible for the verbatim copy, which
 * carries the hash table's next-free index along untouched. */
$negativePlain = [-5 => 1];
$control = $negativePlain;
$cache->store('negative-plain', $negativePlain);
$fetched = $cache->fetch('negative-plain');
ok('plain value', $fetched === $negativePlain);
$control[] = 'x';
$fetched[] = 'x';
ok('plain control keys', array_keys($control) === [-5, -4]);
ok('plain fetched keys', array_keys($fetched) === array_keys($control));
?>
--EXPECT--
negative object survived: OK
negative object control keys: OK
negative object fetched keys: OK
emptied is empty: OK
emptied control keys: OK
emptied fetched keys: OK
huge object survived: OK
huge control keys: -1,0
huge fetched keys: -1,0
huge fetched keys match control: OK
plain value: OK
plain control keys: OK
plain fetched keys: OK
