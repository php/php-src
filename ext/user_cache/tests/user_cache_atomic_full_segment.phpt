--TEST--
UserCache\Cache: increment() and decrement() on a full segment without eviction return null for new counters and still update existing ones, and a request that keeps reading a rewritten value stops pinning payloads after a quarter of the data area
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=2M
user_cache.eviction_policy=none
--FILE--
<?php
$cache = UserCache\Cache::getPool('atomic-full');
$cache->clear();
var_dump($cache->store('counter', 10), $cache->store('expiring-counter', 20, 60));

$n = 0;
foreach ([4000, 1000, 200, 40, 8, 1] as $size) {
    while ($cache->store('fill-' . $n++, str_repeat('x', $size)));
}
$status = UserCache\Cache::getStatus();
var_dump($status->getFreeMemory() < 64, $status->getEvictionCount(), $status->getExpungeCount());
$failures = $status->getStoreFailureCount();

/* A missing counter needs an allocation: capacity failures return null, not an exception */
var_dump($cache->increment('missing'));
var_dump($cache->decrement('missing', 3, 60));
var_dump($cache->has('missing'));
var_dump(UserCache\Cache::getStatus()->getStoreFailureCount() > $failures);

/* Existing integer entries are updated in place on both the lock-free and the locked path */
var_dump($cache->increment('counter', 5));
var_dump($cache->decrement('counter', 20));
var_dump($cache->increment('expiring-counter', 2));
var_dump($cache->decrement('expiring-counter', 1, 60));
var_dump($cache->fetch('counter'), $cache->fetch('expiring-counter'));

/* Nothing was evicted or wiped to make room */
$status = UserCache\Cache::getStatus();
var_dump($status->getEvictionCount(), $status->getExpungeCount());
var_dump($cache->fetch('fill-0') === str_repeat('x', 4000));

function request_pin_budget(): void
{
    $cache = UserCache\Cache::getPool('request-pin-budget');
    $cache->clear();

    $cache->store('small', ['a' => 1, 'b' => [1, 2, 3]]);
    $before = UserCache\Cache::getStatus()->getGraphPinnedReferences();
    $small = $cache->fetch('small');
    var_dump(UserCache\Cache::getStatus()->getGraphPinnedReferences() - $before);

    $value = range(1, 8000);
    $failures = 0;
    $stale = 0;
    $held = [];
    for ($i = 0; $i < 200; $i++) {
        $value[0] = $i;
        if (!$cache->store('hot', $value)) {
            $failures++;
        }

        $fetched = $cache->fetch('hot');
        if ($fetched[0] !== $i || $cache->fetch('hot') !== $fetched) {
            $stale++;
        }

        $held[] = $fetched;
    }

    $status = UserCache\Cache::getStatus();
    var_dump($failures, $stale, $status->getGraphPinnedReferences() - $before < 16);
    var_dump($held[0][0], $held[199][0], $small['b'][2]);
    var_dump($status->getEvictionCount(), $status->getExpungeCount());
}

echo "\nrequest pin budget:\n";
$cache->clear();
request_pin_budget();
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
int(0)
int(0)
NULL
NULL
bool(false)
bool(true)
int(15)
int(-5)
int(22)
int(21)
int(-5)
int(21)
int(0)
int(0)
bool(true)

request pin budget:
int(1)
int(0)
int(0)
bool(true)
int(0)
int(199)
int(3)
int(0)
int(0)
