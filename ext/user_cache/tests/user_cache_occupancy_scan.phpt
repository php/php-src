--TEST--
UserCache\Cache: table scans follow the occupancy bitmap across churn, rehash, expiry and pool deletion
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
/* The segment outlives the request (php --repeat): start from fresh pools. */
UserCache\Cache::deletePool('occupancy-a');
UserCache\Cache::deletePool('occupancy-b');
$a = UserCache\Cache::getPool('occupancy-a');
$b = UserCache\Cache::getPool('occupancy-b');

function keys_of(UserCache\Cache $cache): array {
    $keys = $cache->getPoolStatus()->getEntryKeys();
    sort($keys);

    return $keys;
}

$expected = [];
for ($i = 0; $i < 200; $i++) {
    $a->store("k$i", $i);
    $expected["k$i"] = true;
}
for ($i = 0; $i < 50; $i++) {
    $b->store("b$i", $i);
}

for ($i = 0; $i < 200; $i += 3) {
    $a->delete("k$i");
    unset($expected["k$i"]);
}
for ($i = 1; $i < 200; $i += 3) {
    $a->store("k$i", "again$i");
}

/* Enough tombstones to force a rehash of the whole table. */
$capacity = UserCache\Cache::getStatus()->getEntryCapacity();
$churn = intdiv($capacity, 4) + 64;
for ($i = 0; $i < $churn; $i++) {
    $a->store("churn$i", 1);
}
for ($i = 0; $i < $churn; $i++) {
    $a->delete("churn$i");
}

/* Expired entries are invisible to the scan once reclaimed; the bounded
 * expiry scan runs after a burst of write operations. */
$a->store('ttl', 'x', 1);
sleep(2);
var_dump($a->fetch('ttl', 'gone'));
for ($i = 0; $i < 70; $i++) {
    $b->store("w$i", $i);
}

$keys = array_keys($expected);
sort($keys);
var_dump(keys_of($a) === $keys);
var_dump($a->getPoolStatus()->getEntryCount() === count($keys));
var_dump(count(keys_of($b)));

var_dump(UserCache\Cache::deletePool('occupancy-b'));
var_dump(keys_of(UserCache\Cache::getPool('occupancy-b')));
var_dump(keys_of($a) === $keys);

var_dump($a->clear());
var_dump(keys_of($a));
var_dump(UserCache\Cache::getStatus()->getEntryCount());

var_dump($a->store('after', 1));
var_dump(keys_of($a));
?>
--EXPECT--
string(4) "gone"
bool(true)
bool(true)
int(120)
bool(true)
array(0) {
}
bool(true)
bool(true)
array(0) {
}
int(0)
bool(true)
array(1) {
  [0]=>
  string(5) "after"
}
