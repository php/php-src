--TEST--
UserCache\Cache: per-key records preserve colliding, binary and long keys across pool lifecycles
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=16M
--FILE--
<?php
/* Match the low bits of Zend's string hash without platform-size arithmetic. */
function lowHash(string $key): int
{
    $hash = 5381 & 255;
    for ($i = 0; $i < strlen($key); $i++) {
        $hash = ($hash * 33 + ord($key[$i])) & 255;
    }
    return $hash;
}

$cache = UserCache\Cache::getPool("key-cache\0pool");
$other = UserCache\Cache::getPool('key-cache-other');
$keys = [];
$hashes = [];
for ($i = 0; count($keys) < 16; $i++) {
    $key = "binary\0\xff:" . $i;
    $hash = lowHash($key);
    if (($hash & 15) === 0 && !isset($hashes[$hash])) {
        $keys[] = $key;
        $hashes[$hash] = true;
    }
}

$ok = true;
foreach ($keys as $i => $key) {
    $ok = $cache->store($key, $i) && $other->store($key, -$i - 1) && $ok;
}
/* Keys whose hashes share their low bits still resolve to their own records. */
for ($round = 0; $round < 16; $round++) {
    foreach (array_reverse($keys, true) as $i => $key) {
        $copy = substr('!' . $key, 1);
        $ok = $cache->fetch($copy) === $i && $other->fetch($copy) === -$i - 1 && $ok;
    }
}
var_dump($ok);

/* Promote the inline records to a table, grow it, and revisit earlier keys. */
$values = [];
for ($i = 0; $i < 4096; $i++) {
    $values['working-set:' . $i] = $i;
}
var_dump($cache->storeMultiple($values));
var_dump($cache->fetchMultiple(array_keys($values)) === $values);
$ok = true;
foreach (array_reverse($values, true) as $key => $value) {
    $copy = substr('!' . $key, 1);
    $ok = $cache->fetch($copy) === $value && $ok;
}
foreach ($keys as $i => $key) {
    $ok = $cache->fetch($key) === $i && $ok;
}
var_dump($ok);

/* Keys sharing a hash bucket still compare by content. */
$colliding = [];
for ($i = 0; count($colliding) < 4; $i++) {
    $key = 'same-slot:' . $i;
    if (lowHash($key) === 0) {
        $colliding[$key] = $i;
    }
}
var_dump($cache->storeMultiple($colliding));
var_dump($cache->fetchMultiple(array_keys($colliding)) === $colliding);

$long = str_repeat('long', 16000) . "\0tail";
var_dump($cache->store($long, 123), $cache->fetch($long), $cache->has($long));
var_dump($cache->delete($long), $cache->fetch($long, 'missing'));
$longPool = UserCache\Cache::getPool(str_repeat('pool', 1024));
var_dump($longPool->store('short', 7), $longPool->fetch('short'));

/* The pool name, the delimiter and the key together are at most 65535 bytes. */
$limit = 65535 - strlen("key-cache\0pool") - 1;
var_dump($cache->store(str_repeat('k', $limit), 'at the limit'), $cache->fetch(str_repeat('k', $limit)));
$tooLong = str_repeat('k', $limit + 1);
foreach ([
    fn () => $cache->store($tooLong, 1),
    fn () => $cache->fetchMultiple([$tooLong]),
    fn () => $cache->storeMultiple([$tooLong => 1]),
] as $call) {
    try {
        $call();
    } catch (ValueError $e) {
        echo $e->getMessage(), "\n";
    }
}
try {
    UserCache\Cache::getPool(str_repeat('p', 65535 - strlen((string) PHP_INT_MIN)));
} catch (ValueError $e) {
    var_dump($e->getMessage() === 'UserCache\Cache::getPool(): Argument #1 ($pool) must not be longer than ' . (65534 - strlen((string) PHP_INT_MIN)) . ' bytes');
}
$longestPool = UserCache\Cache::getPool(str_repeat('p', 65534 - strlen((string) PHP_INT_MIN)));
var_dump($longestPool->store(PHP_INT_MIN, 'integer key'), $longestPool->fetch(PHP_INT_MIN));

var_dump(UserCache\Cache::deletePool("key-cache\0pool"));
$new = UserCache\Cache::getPool("key-cache\0pool");
var_dump($new !== $cache, $new->fetch($keys[0], 'missing'));
var_dump($cache->store($keys[0], 900), $new->fetch($keys[0]));
var_dump($other->fetch($keys[0]));
var_dump($new->clear(), $cache->fetch($keys[0], 'missing'));

foreach (['', "invalid\x1fkey"] as $invalid) {
    try {
        $cache->fetch($invalid);
    } catch (ValueError $exception) {
        echo "invalid key rejected\n";
    }
}
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
int(123)
bool(true)
bool(true)
string(7) "missing"
bool(true)
int(7)
bool(true)
string(12) "at the limit"
UserCache\Cache::store(): Argument #1 ($key) must not be longer than 65520 bytes
UserCache\Cache::fetchMultiple(): Argument #1 ($keys) must contain only cache keys that are not longer than 65520 bytes
UserCache\Cache::storeMultiple(): Argument #1 ($values) must be an array whose keys are not longer than 65520 bytes
bool(true)
bool(true)
string(11) "integer key"
bool(true)
bool(true)
string(7) "missing"
bool(true)
int(900)
int(-1)
bool(true)
string(7) "missing"
invalid key rejected
invalid key rejected
