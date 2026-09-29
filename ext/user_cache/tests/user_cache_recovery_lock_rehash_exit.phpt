--TEST--
UserCache\Cache: a process dying while it compacts the lock table loses no live lock, leaves no duplicate and is recovered by the next locked read
--SKIPIF--
<?php
if (!PHP_DEBUG) die('skip requires a debug build (fault injection is ZEND_DEBUG-only)');
?>
--EXTENSIONS--
pcntl
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=1M
user_cache.entries_hint=127
--FILE--
<?php
/* Low bits of the storage key's DJB hash select the slot of the 128-slot lock table. */
function lock_slot(string $key): int
{
    $h = 5381;
    foreach (str_split("recovery-lock-rehash\x1f$key") as $c) {
        $h = ($h * 33 + ord($c)) & 127;
    }

    return $h;
}

function key_at(string $prefix, int $slot, array $avoid = []): string
{
    for ($i = 0; ; $i++) {
        if (lock_slot("$prefix$i") === $slot && !in_array("$prefix$i", $avoid, true)) {
            return "$prefix$i";
        }
    }
}

function child_locks(UserCache\Cache $cache, string $key): bool
{
    $pid = pcntl_fork();
    if ($pid === 0) {
        exit($cache->lock($key) ? 1 : 0);
    }
    pcntl_waitpid($pid, $status);

    return pcntl_wexitstatus($status) === 1;
}

$cache = UserCache\Cache::getPool('recovery-lock-rehash');
$cache->clear();
$cache->store('value', 'written before the crash');

/* 'held' sits one slot past its home; the home slot becomes a tombstone. */
$first = key_at('first-', 40);
$held = key_at('held-', 40);
var_dump($cache->lock($first), $cache->lock($held), $cache->unlock($first));

/* Tombstones on 31 other homes reach the compaction threshold of 128 / 4. */
$pid = pcntl_fork();
if ($pid === 0) {
    $slot = 60;
    for ($n = 0; $n < 31; $n++, $slot++) {
        $key = key_at('churn-', $slot);
        if (!$cache->lock($key) || !$cache->unlock($key)) {
            exit(2);
        }
    }
    /* The next lookup compacts the table and moves 'held' home, then dies. */
    putenv('USER_CACHE_DEBUG_EXIT_IN_ENTRY_LOCK_REHASH=1');
    $cache->lock(key_at('churn-', $slot));
    exit(1);
}
pcntl_waitpid($pid, $status);
var_dump(pcntl_wifexited($status) && pcntl_wexitstatus($status) === 0);

/* A read under the global lock must recover the half-compacted table before it reopens lock-free scalar writes. */
$pid = pcntl_fork();
if ($pid === 0) {
    putenv('USER_CACHE_DEBUG_FORCE_LOCKED_FETCH=1');
    echo 'locked read: ', UserCache\Cache::getPool('recovery-lock-rehash')->fetch('value', 'recovered first'), "\n";
    exit(0);
}
pcntl_waitpid($pid, $status);

var_dump(child_locks($cache, $held));
var_dump($cache->unlock($held));
var_dump(child_locks($cache, $held));
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
locked read: recovered first
bool(false)
bool(true)
bool(true)
