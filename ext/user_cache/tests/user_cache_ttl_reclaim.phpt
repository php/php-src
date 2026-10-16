--TEST--
UserCache\Cache: expiry reclaim survives TTL transitions, bulk rollback and tombstone rehash, and entry counts include expired entries until they are reclaimed
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=1M
user_cache.entries_hint=127
user_cache.eviction_policy=none
--FILE--
<?php
/* Counter expiry and TTL overwrite semantics at fetch level */
$semantics = UserCache\Cache::getPool('ttl-semantics');
$semantics->clear();
var_dump($semantics->increment('counter', 1, 1));
var_dump($semantics->store('overwritten', 'v1', 1));
var_dump($semantics->store('overwritten', 'v2'));
var_dump($semantics->store('shortened', 'v1'));
var_dump($semantics->store('shortened', 'v2', 1));

/* TTL transitions and bulk rollback reclaimed by the write sweep */
$cache = UserCache\Cache::getPool('ttl-transitions');
$cache->clear();
$cache->store('persistent', 1, 1);
$cache->store('persistent', 2);
$cache->store('expired', 1);
$cache->store('expired', 2, 1);
$cache->store('rollback', 3, 1);
var_dump($cache->storeMultiple([
    'rollback' => 4,
    'too-large' => str_repeat('x', 4 * 1024 * 1024),
]));
$cache->store('deleted', 1, 1);
$cache->delete('deleted');
$cache->increment('counter', 1, 1);
$cache->increment('counter', 1);
$other = UserCache\Cache::getPool('ttl-cleared');
$other->store('removed', 1, 1);
$other->clear();

sleep(2);

var_dump($semantics->fetch('counter', 'MISS'));
var_dump($semantics->increment('counter'));
var_dump($semantics->fetch('overwritten', 'MISS'));
var_dump($semantics->fetch('shortened', 'MISS'));

for ($i = 0; $i < 128; $i++) {
    $cache->store('write', $i);
}
$keys = $cache->getPoolStatus()->getEntryKeys();
sort($keys);
var_dump($keys, $cache->fetch('persistent'));

$cache->clear();
$cache->store('expired-after-clear', 1, 1);
sleep(2);
for ($i = 0; $i < 128; $i++) {
    $cache->store('write', $i);
}
var_dump($cache->getPoolStatus()->getEntryKeys());

/* Delete churn rehashes the table while TTL tracking stays active */
$cache = UserCache\Cache::getPool('ttl-rehash');
$cache->clear();
var_dump($cache->store('expired', 1, 1));
for ($i = 0; $i < 50; $i++) {
    $cache->store("keep_$i", ['id' => $i, 'name' => "keeper_$i"]);
}
for ($round = 0; $round < 10; $round++) {
    for ($i = 0; $i < 80; $i++) {
        $cache->store("churn_{$round}_{$i}", $i);
    }
    for ($i = 0; $i < 80; $i++) {
        $cache->delete("churn_{$round}_{$i}");
    }
}
var_dump(UserCache\Cache::getStatus()->getTombstoneCount() < 80);

$ok = true;
for ($i = 0; $i < 50; $i++) {
    $value = $cache->fetch("keep_$i");
    if (!is_array($value) || $value['id'] !== $i || $value['name'] !== "keeper_$i") {
        $ok = false;
        echo "lost keep_$i\n";
    }
}
var_dump($ok);
var_dump($cache->fetch('churn_0_0', 'gone'));
var_dump($cache->has('churn_9_79'));
var_dump($cache->store('after_rehash', 'value'));
var_dump($cache->fetch('after_rehash'));

sleep(2);
for ($i = 0; $i < 128; $i++) {
    $cache->store('live', $i);
}
$expected = ['after_rehash', 'live'];
for ($i = 0; $i < 50; $i++) {
    $expected[] = "keep_$i";
}
sort($expected);
$keys = $cache->getPoolStatus()->getEntryKeys();
sort($keys);
var_dump($keys === $expected);

function expired_entries_counted(): void
{
    $cache = UserCache\Cache::getPool('ttl-counted');
    $cache->clear();
    $base = UserCache\Cache::getStatus()->getEntryCount();
    var_dump($cache->store('short-a', 1, 1), $cache->store('short-b', 'b', 1), $cache->store('kept', 0));
    var_dump(UserCache\Cache::getStatus()->getEntryCount() - $base, $cache->getPoolStatus()->getEntryCount());

    usleep(1200000);
    var_dump($cache->has('short-a'), $cache->fetch('short-b', 'MISS'));
    $pool_status = $cache->getPoolStatus();
    $keys = $pool_status->getEntryKeys();
    sort($keys);
    var_dump(UserCache\Cache::getStatus()->getEntryCount() - $base, $pool_status->getEntryCount(), $keys);

    for ($i = 0; $i < 128; $i++) {
        $cache->store('kept', $i);
    }
    var_dump(UserCache\Cache::getStatus()->getEntryCount() - $base, $cache->getPoolStatus()->getEntryCount());
}

echo "\nexpired entries counted:\n";
expired_entries_counted();
?>
--EXPECT--
int(1)
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
string(4) "MISS"
int(1)
string(2) "v2"
string(4) "MISS"
array(2) {
  [0]=>
  string(10) "persistent"
  [1]=>
  string(5) "write"
}
int(2)
array(1) {
  [0]=>
  string(5) "write"
}
bool(true)
bool(true)
bool(true)
string(4) "gone"
bool(false)
bool(true)
string(5) "value"
bool(true)

expired entries counted:
bool(true)
bool(true)
bool(true)
int(3)
int(3)
bool(false)
string(4) "MISS"
int(3)
int(3)
array(3) {
  [0]=>
  string(4) "kept"
  [1]=>
  string(7) "short-a"
  [2]=>
  string(7) "short-b"
}
int(1)
int(1)
