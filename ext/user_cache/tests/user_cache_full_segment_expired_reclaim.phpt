--TEST--
UserCache\Cache: a full segment without eviction reclaims expired entries once the earliest deadline is due, also for a shorter TTL stored after a sweep, and lock() reclaims key space held by expired lock records
--EXTENSIONS--
pcntl
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=1M
user_cache.eviction_policy=none
--FILE--
<?php
function fill(UserCache\Cache $cache, string $prefix, string $value, int $ttl): int
{
    $stored = 0;
    while ($cache->store(sprintf('%s-%04d', $prefix, $stored), $value, $ttl)) {
        $stored++;
    }

    return $stored;
}

function ttl_expiry_floor(): void
{
    $cache = UserCache\Cache::getPool('expiry-floor');
    $value = str_repeat('v', 2000);

    /* Short TTLs first, then long TTLs until the segment is full: nothing has expired yet, so the store fails */
    for ($i = 0; $i < 10; $i++) {
        $cache->store(sprintf('shrt-%04d', $i), $value, 1);
    }
    $long = fill($cache, 'long', $value, 3600);
    var_dump($long > 100);

    sleep(2);

    /* The short TTLs are due: a store under pressure sweeps them and the sweep records the long deadlines */
    $after = 0;
    for ($i = 0; $i < 10; $i++) {
        $after += $cache->store(sprintf('aft1-%04d', $i), $value, 3600) ? 1 : 0;
    }
    var_dump($after, $cache->has('shrt-0000'), $cache->has('long-0000'));

    /* A shorter TTL stored after that sweep lowers the recorded deadline again */
    for ($i = 0; $i < 10; $i++) {
        $cache->delete(sprintf('long-%04d', $i));
    }
    $short = 0;
    for ($i = 0; $i < 10; $i++) {
        $short += $cache->store(sprintf('shr2-%04d', $i), $value, 1) ? 1 : 0;
    }
    var_dump($short, fill($cache, 'xtra', $value, 3600));

    sleep(2);

    $after = 0;
    for ($i = 0; $i < 10; $i++) {
        $after += $cache->store(sprintf('aft2-%04d', $i), $value, 3600) ? 1 : 0;
    }
    $status = UserCache\Cache::getStatus();
    var_dump($after, $cache->has('shr2-0000'), $cache->has('long-0010'), $status->getEvictionCount(), $status->getExpungeCount());
}

/* Low bits of the storage key's DJB hash select the lock-table slot. */
function lock_slot(string $pool, string $key): int
{
    $h = 5381;
    foreach (str_split("$pool\x1f$key") as $c) {
        $h = ($h * 33 + ord($c)) & 127;
    }

    return $h;
}

function lock_full_segment(): void
{
    $cache = UserCache\Cache::getPool('lock-space');
    $cache->clear();

    /* Leased locks outlive their holder as ownerless records until they expire. */
    $pid = pcntl_fork();
    if ($pid === 0) {
        $ok = true;
        for ($i = 0; $i < 8; $i++) {
            $ok = $cache->lock("stale-$i", 1) && $ok;
        }
        exit($ok ? 0 : 1);
    }
    pcntl_waitpid($pid, $status);
    var_dump(pcntl_wifexited($status) && pcntl_wexitstatus($status) === 0);

    /* The new key's lock-table chain must not pass the stale records. */
    $stale = [];
    for ($i = 0; $i < 8; $i++) {
        $stale[lock_slot('lock-space', "stale-$i")] = true;
    }
    for ($i = 0; isset($stale[lock_slot('lock-space', "fresh-$i")]); $i++);
    $fresh = "fresh-$i";

    $n = 0;
    foreach ([4000, 1000, 200, 40, 8] as $size) {
        while ($cache->store('fill-' . $n++, str_repeat('x', $size)));
    }
    var_dump(UserCache\Cache::getStatus()->getFreeMemory() < 64);

    sleep(2);
    var_dump($cache->lock($fresh));
    var_dump($cache->unlock($fresh));
}

echo "ttl expiry floor:\n";
ttl_expiry_floor();

echo "\nlock full segment:\n";
UserCache\Cache::getPool('expiry-floor')->clear();
lock_full_segment();
?>
--EXPECT--
ttl expiry floor:
bool(true)
int(10)
bool(false)
bool(true)
int(10)
int(0)
int(10)
bool(false)
bool(true)
int(0)
int(0)

lock full segment:
bool(true)
bool(true)
bool(true)
bool(true)
