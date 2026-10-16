--TEST--
UserCache\Cache: LRU eviction keeps hot and locked entries and absorbs size-class and slot pressure without clearing
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=4M
user_cache.entries_hint=2048
--FILE--
<?php
require __DIR__ . '/user_cache_clock.inc';

/* Recently fetched and locked entries survive memory pressure while cold neighbours are evicted. */
$cache = UserCache\Cache::getPool('eviction-lru');
$cache->clear();
$base = UserCache\Cache::getStatus();
$blob = str_repeat('x', 8192);

$stored = 0;
for ($i = 0; $i < 476; $i++) {
    if (!$cache->store('k' . $i, $blob . $i)) {
        break;
    }
    $stored++;
}
var_dump($stored > 400);
var_dump($cache->lock('k3'));

sleep(1);
$cache->store('clock', 'tick', 60);

for ($i = 10; $i < 20; $i++) {
    $cache->fetch('k' . $i);
}

$pressured_ok = 0;
for ($i = 0; $i < 30; $i++) {
    if ($cache->store('new' . $i, $blob . 'n' . $i)) {
        $pressured_ok++;
    }
}
var_dump($pressured_ok === 30);

$touched_alive = 0;
for ($i = 10; $i < 20; $i++) {
    if ($cache->fetch('k' . $i) !== null) {
        $touched_alive++;
    }
}
$new_alive = 0;
for ($i = 0; $i < 30; $i++) {
    if ($cache->has('new' . $i)) {
        $new_alive++;
    }
}

$status = UserCache\Cache::getStatus();
var_dump($touched_alive === 10);
var_dump($new_alive === 30);
var_dump($status->getEntryCount() > 400);
var_dump($status->getEvictionCount() > $base->getEvictionCount());
var_dump($status->getExpungeCount() === $base->getExpungeCount());
var_dump($status->getStoreFailureCount() === $base->getStoreFailureCount());

for ($i = 0; $i < 600; $i++) {
    $cache->store('churn' . $i, $blob . 'c' . $i);
}

var_dump($cache->fetch('k3') !== null);
var_dump($cache->fetch('k2', 'MISS') === 'MISS');
var_dump($cache->fetch('k4', 'MISS') === 'MISS');
var_dump(UserCache\Cache::getStatus()->getExpungeCount() === $base->getExpungeCount());

var_dump($cache->unlock('k3'));
$cache->clear();

/* A value one size class above the resident 8320-byte blocks evicts about one neighbour each. */
$cache = UserCache\Cache::getPool('eviction-straddle');
$cache->clear();
$blob = str_repeat('x', 8196);

$start = start_on_fresh_second();
for ($i = 0; $i < 476; $i++) {
    $cache->store('k' . $i, $blob . $i);
}

$base = UserCache\Cache::getStatus();

for ($i = 100; $i < 700; $i++) {
    if (!$cache->store('wide' . $i, $blob . 'n' . $i)) {
        exit("store failed at $i\n");
    }
}

/* Neighbours stamped in a later second are refused, so extra victims or a clear may follow. */
$split = crossed_second($start);

$status = UserCache\Cache::getStatus();
$evictions = $status->getEvictionCount() - $base->getEvictionCount();

var_dump($split || $evictions <= 600 + 600 / 50);
var_dump($split || $status->getWastedMemory() < 64 * 1024);
var_dump($split || $status->getExpungeCount() === $base->getExpungeCount());
var_dump($status->getStoreFailureCount() === $base->getStoreFailureCount());
var_dump($cache->fetch('wide699') === $blob . 'n699');
$cache->clear();

/* A full entry table combined with full memory evicts instead of clearing. */
$randomizer = new Random\Randomizer(new Random\Engine\Xoshiro256StarStar(7));
$cache = UserCache\Cache::getPool('eviction-slots');
$cache->clear();
$base = UserCache\Cache::getStatus();
$sizes = [100, 300, 700, 1100, 1500, 2100, 3000, 3900];

for ($i = 0; $i < 6000; $i++) {
    if (!$cache->store('m' . $i, str_repeat('x', $sizes[$randomizer->getInt(0, 7)] + $randomizer->getInt(0, 300)))) {
        exit("store failed at $i\n");
    }
}

$status = UserCache\Cache::getStatus();
var_dump($status->getEvictionCount() > $base->getEvictionCount());
var_dump($status->getExpungeCount() === $base->getExpungeCount());
var_dump($status->getStoreFailureCount() === $base->getStoreFailureCount());
var_dump($status->getEntryCount() > 1000);
$cache->clear();
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
