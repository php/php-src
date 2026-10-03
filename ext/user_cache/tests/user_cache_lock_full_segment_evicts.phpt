--TEST--
UserCache\Cache: under LRU a full segment makes room for lock() like for a store, clears the cache when only an expired lock record kept it fragmented, and counts each evicted entry once
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=1M
user_cache.eviction_policy=lru
--FILE--
<?php
use Random\Engine\Xoshiro256StarStar;
use Random\Randomizer;
use UserCache\Cache;

function value(Randomizer $randomizer): string|int|array
{
    switch ($randomizer->getInt(0, 5)) {
        case 0:
            return str_repeat('a', $randomizer->getInt(1, 200));
        case 1:
            return str_repeat('b', $randomizer->getInt(300, 5000));
        case 2:
            return str_repeat('c', $randomizer->getInt(5000, 60000));
        case 3:
            return range(1, $randomizer->getInt(1, 300));
        case 4:
            return $randomizer->getInt(PHP_INT_MIN, PHP_INT_MAX);
        default:
            $value = [];
            for ($i = $randomizer->getInt(1, 50); $i > 0; $i--) {
                $value["k$i"] = str_repeat('x', $randomizer->getInt(0, 400));
            }

            return $value;
    }
}

function eviction_count_matches_removed(): void
{
    foreach ([1, 2, 4] as $seed) {
        $randomizer = new Randomizer(new Xoshiro256StarStar($seed));
        $cache = Cache::getPool("eviction-count-$seed");
        $mismatches = 0;

        for ($op = 0; $op < 3000; $op++) {
            $key = 'k' . $randomizer->getInt(0, 399);
            $before = Cache::getStatus();
            $existed = $cache->has($key);
            $stored = $cache->store($key, value($randomizer));
            $after = Cache::getStatus();

            if (!$stored || $existed || $after->getExpungeCount() !== $before->getExpungeCount()) {
                continue;
            }

            $removed = $before->getEntryCount() + 1 - $after->getEntryCount();
            if ($after->getEvictionCount() - $before->getEvictionCount() !== $removed) {
                $mismatches++;
            }
        }

        echo "seed $seed: $mismatches mismatches\n";
        $cache->clear();
    }
}

function eviction_clear_after_expired_lock(): void
{
    $cache = Cache::getPool('eviction-clear-after-expired-lock');
    $cache->clear();

    var_dump($cache->store('front', str_repeat('f', 350000)));
    var_dump($cache->lock('expired', 1));
    var_dump($cache->store('back', str_repeat('b', 350000)));

    sleep(2);

    $before = Cache::getStatus();
    var_dump($cache->store('huge', str_repeat('h', 700000)));
    $after = Cache::getStatus();

    var_dump($after->getExpungeCount() - $before->getExpungeCount());
    var_dump(strlen($cache->fetch('huge')));
    var_dump($cache->has('front'), $cache->has('back'));
}

echo "eviction count:\n";
eviction_count_matches_removed();

echo "\nlock evicts:\n";
$cache = UserCache\Cache::getPool('lock-evicts');
$cache->clear();

for ($i = 0; $i < 400; $i++) {
    $cache->store("fill-$i", str_repeat('x', 4000));
}

/* The lock key cannot fit into what is left without evicting entries. */
$free = UserCache\Cache::getStatus()->getFreeMemory();
var_dump($free < 8000);

$key = str_repeat('k', $free + 1000);
var_dump($cache->lock($key), $cache->unlock($key));
var_dump($cache->remember($key, fn () => 'computed'), $cache->fetch($key));

echo "\nclear after expired lock:\n";
$cache->clear();
eviction_clear_after_expired_lock();
?>
--EXPECT--
eviction count:
seed 1: 0 mismatches
seed 2: 0 mismatches
seed 4: 0 mismatches

lock evicts:
bool(true)
bool(true)
bool(true)
string(8) "computed"
string(8) "computed"

clear after expired lock:
bool(true)
bool(true)
bool(true)
bool(true)
int(1)
int(700000)
bool(false)
bool(false)
