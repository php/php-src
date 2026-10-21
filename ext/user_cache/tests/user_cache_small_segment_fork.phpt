--TEST--
UserCache\Cache: a 1 MiB segment shared with forked children keeps LRU eviction of a small entry table on the oldest candidates, lets a lock table filled by a dead owner be taken over, keeps a dead owner's leased lock guarding its key from LRU eviction until the lease expires, and does not make remember() wait for a slot of a full lock table
--EXTENSIONS--
pcntl
--SKIPIF--
<?php
if (!is_executable('/bin/true')) die('skip needs /bin/true');
?>
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=1M
--FILE--
<?php
use UserCache\Cache;

require __DIR__ . '/user_cache_clock.inc';

function eviction_lru_small_table(): void
{
    $cache = Cache::getPool('eviction-lru-small-table');
    $chunk = str_repeat('v', 45 * 1024);
    for ($i = 0; $i < 40; $i++) {
        if (Cache::getStatus()->getFreeMemory() < 47 * 1024) {
            break;
        }
        $cache->store("k$i", $chunk . $i);
    }
    $stored = $i;
    sleep(1);

    /* A later reader makes the odd entries newer than the even ones. */
    $pid = pcntl_fork();
    if ($pid === 0) {
        $child = Cache::getPool('eviction-lru-small-table');
        $child->store('tick', 1, 100);
        $child->delete('tick');
        for ($i = 1; $i < $stored; $i += 2) {
            $child->fetch("k$i");
        }
        exit(0);
    }
    pcntl_waitpid($pid, $status);

    $before = Cache::getStatus();
    var_dump($cache->store('big', str_repeat('B', 100 * 1024)));
    $after = Cache::getStatus();
    $evicted = $after->getEvictionCount() - $before->getEvictionCount();
    $left = 0;
    for ($i = 0; $i < $stored; $i++) {
        $left += (int) $cache->has("k$i");
    }
    var_dump(
        $after->getExpungeCount() === $before->getExpungeCount(),
        $evicted > 1,
        $left === $stored - $evicted,
        $left > $stored / 2
    );
}

function lock_table_dead_owners(): void
{
    $cache = UserCache\Cache::getPool('lock-dead-owners');
    $cache->clear();

    $pid = pcntl_fork();
    if ($pid === 0) {
        $child = UserCache\Cache::getPool('lock-dead-owners');
        $held = 0;
        for ($i = 0; $i < 200; $i++) {
            $held += $child->lock("dead-$i") ? 1 : 0;
        }
        echo "child holds $held locks\n";
        /* exec() replaces the process without the request shutdown that would release the locks. */
        pcntl_exec('/bin/true');
        exit(1);
    }
    pcntl_waitpid($pid, $status);

    var_dump($cache->lock('fresh'), $cache->unlock('fresh'));
    var_dump($cache->lock('dead-0'), $cache->unlock('dead-0'));
    var_dump($cache->lock('dead-127'), $cache->unlock('dead-127'));
}

function dead_owner_lease_eviction(): void
{
    $cache = Cache::getPool('dead-owner-lease-eviction');
    $cache->clear();
    $chunk = str_repeat('g', 45 * 1024);
    var_dump(
        $cache->store('guarded', $chunk . 'guarded'),
        $cache->store('unleased', $chunk . 'unleased'),
        $cache->store('unlocked', $chunk . 'unlocked')
    );

    $forked_at = hrtime();
    $pid = pcntl_fork();
    if ($pid === 0) {
        $child = Cache::getPool('dead-owner-lease-eviction');
        if ($child->lock('guarded', 2) && $child->lock('unleased')) {
            pcntl_exec('/bin/true');
        }
        exit(1);
    }
    pcntl_waitpid($pid, $status);
    $exited_at = hrtime();
    var_dump(pcntl_wexitstatus($status));
    sleep(1);

    $before = Cache::getStatus();
    $stored = 0;
    for ($i = 0; $i < 60; $i++) {
        $stored += (int) $cache->store("churn-$i", $chunk . $i);
    }
    $after = Cache::getStatus();
    var_dump(
        $stored,
        $after->getEvictionCount() > $before->getEvictionCount(),
        $after->getExpungeCount() === $before->getExpungeCount()
    );
    var_dump(
        $cache->has('guarded') || seconds_since($forked_at) >= 2,
        $cache->has('unleased'),
        $cache->has('unlocked')
    );

    usleep(max(0, (int) ((2.3 - seconds_since($exited_at)) * 1000000)));
    for ($i = 0; $i < 60; $i++) {
        $stored += (int) $cache->store("late-churn-$i", $chunk . $i);
    }
    var_dump($stored, $cache->has('guarded'));
    $cache->clear();
}

function remember_lock_table_full(): void
{
    $cache = UserCache\Cache::getPool('remember-lock-table-full');

    $held = 0;
    while ($held < 5000 && $cache->lock("held-$held")) {
        $held++;
    }
    var_dump($held > 0 && $held < 5000);

    $pid = pcntl_fork();
    if ($pid < 0) die('fork failed');
    if ($pid === 0) {
        $calls = 0;
        $start = hrtime(true);
        $value = $cache->remember('fresh', function () use (&$calls) {
            $calls++;

            return 42;
        });
        $elapsed = (hrtime(true) - $start) / 1e9;

        printf("remember=%d calls=%d fast=%s lock=%s\n", $value, $calls, $elapsed < 5.0 ? 'yes' : 'no', var_export($cache->lock('fresh-lock'), true));
        exit(0);
    }

    pcntl_waitpid($pid, $status);
    var_dump(pcntl_wexitstatus($status));
}

/* The segment outlives the request (php --repeat): start from fresh pools. */
foreach ([
            'eviction-lru-small-table',
            'lock-dead-owners',
            'dead-owner-lease-eviction',
            'remember-lock-table-full'
        ] as $pool) {
    Cache::deletePool($pool);
}

echo "eviction lru small table:\n";
eviction_lru_small_table();

echo "\nlock table dead owners:\n";
Cache::getPool('eviction-lru-small-table')->clear();
lock_table_dead_owners();

echo "\ndead owner lease eviction:\n";
dead_owner_lease_eviction();

echo "\nremember lock table full:\n";
$dead = Cache::getPool('lock-dead-owners');
for ($i = 0; $i < 200; $i++) {
    if ($dead->lock("dead-$i")) {
        $dead->unlock("dead-$i");
    }
}
$dead->clear();
remember_lock_table_full();
?>
--EXPECT--
eviction lru small table:
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

lock table dead owners:
child holds 128 locks
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

dead owner lease eviction:
bool(true)
bool(true)
bool(true)
int(0)
int(60)
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
int(120)
bool(false)

remember lock table full:
bool(true)
remember=42 calls=1 fast=yes lock=false
int(0)
