--TEST--
UserCache\Cache: a lock-free scalar write waits for recovery of a half-compacted lock table instead of passing a live entry lock
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
    foreach (str_split("recovery-lock-stripe\x1f$key") as $c) {
        $h = ($h * 33 + ord($c)) & 127;
    }

    return $h;
}

function key_at(string $prefix, int $slot): string
{
    for ($i = 0; ; $i++) {
        if (lock_slot("$prefix$i") === $slot) {
            return "$prefix$i";
        }
    }
}

function run_child(callable $body, array $faults = []): void
{
    $pid = pcntl_fork();
    if ($pid === 0) {
        foreach ($faults as $fault) {
            putenv("USER_CACHE_DEBUG_$fault=1");
        }
        $body();
        exit(0);
    }
    pcntl_waitpid($pid, $status);
}

$cache = UserCache\Cache::getPool('recovery-lock-stripe');
$cache->clear();

$held = key_at('held-', 40);

/* The second store of the same scalar key enables lock-free scalar writes. */
var_dump($cache->store($held, 1), $cache->store($held, 1));

/* 'moved' and 'held' each sit one slot past a tombstoned home slot. */
$left = key_at('left-', 20);
$moved = key_at('moved-', 20);
$first = key_at('first-', 40);
var_dump($cache->lock($left), $cache->lock($moved), $cache->unlock($left));
var_dump($cache->lock($first), $cache->lock($held), $cache->unlock($first));

/* Compaction empties both homes, moves 'moved' and dies before 'held': an empty slot now hides 'held'. */
run_child(function () use ($cache) {
    for ($n = 0, $slot = 60; $n < 30; $n++, $slot++) {
        $key = key_at('churn-', $slot);
        $cache->lock($key);
        $cache->unlock($key);
    }
    putenv('USER_CACHE_DEBUG_EXIT_IN_ENTRY_LOCK_REHASH=1');
    $cache->lock(key_at('churn-', $slot));
});

/* A locked read notices the dead compaction, releases the global lock and dies before it recovers. */
run_child(function () {
    UserCache\Cache::getPool('recovery-lock-stripe')->fetch('any');
}, ['FORCE_LOCKED_FETCH', 'EXIT_BEFORE_READ_LOCK_RECOVERY']);

[$result_read, $result_write] = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, STREAM_IPPROTO_IP);
$writer = pcntl_fork();
if ($writer === 0) {
    fclose($result_read);
    fwrite($result_write, var_export(UserCache\Cache::getPool('recovery-lock-stripe')->increment($held), true));
    exit(0);
}
fclose($result_write);

usleep(300000);
var_dump($cache->store($held, 100), $cache->unlock($held));
pcntl_waitpid($writer, $status);
echo 'increment after the owner released: ', stream_get_contents($result_read), "\n";
var_dump($cache->fetch($held));
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
increment after the owner released: 101
int(101)
