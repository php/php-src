--TEST--
Leased locks are bounded by user_cache.lock_lease_max, a per-segment count and a key byte budget
--EXTENSIONS--
pcntl
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=1M
user_cache.entries_hint=10
user_cache.lock_lease_max=2
--FILE--
<?php

$pool = UserCache\Cache::getPool('lease');
$pool->store('warm', 1);

function inChild(callable $body): string
{
    $out = tempnam(sys_get_temp_dir(), 'ucache_lease');
    $pid = pcntl_fork();
    if ($pid === 0) {
        file_put_contents($out, $body());
        exit(0);
    }
    pcntl_waitpid($pid, $status);
    $result = file_get_contents($out);
    unlink($out);

    return $result;
}

echo "leased lock count per segment\n";
echo inChild(function () use ($pool) {
    $acquired = 0;
    for ($i = 0; $i < 48; $i++) {
        if ($pool->lock('counted' . $i, 2)) {
            $acquired++;
        }
    }
    $unleased = $pool->lock('unleased', 0);

    return "acquired=$acquired unleased=" . var_export($unleased, true);
}), "\n";
usleep(2200000);

echo "lock key byte budget\n";
echo inChild(function () use ($pool) {
    $first = $pool->lock(str_repeat('a', 40000), 2);
    $second = $pool->lock(str_repeat('b', 40000), 2);
    $unleased = $pool->lock(str_repeat('c', 40000), 0);

    return 'first=' . var_export($first, true) . ' second=' . var_export($second, true) . ' unleased=' . var_export($unleased, true);
}), "\n";
usleep(2200000);

echo "lease clamped to user_cache.lock_lease_max\n";
var_dump($pool->lock('clamped', PHP_INT_MAX));
$start = microtime(true);
echo inChild(function () use ($pool) {
    for ($i = 0; $i < 120; $i++) {
        if ($pool->lock('clamped', 0)) {
            return 'taken over';
        }
        usleep(50000);
    }

    return 'still held';
}), "\n";
var_dump(microtime(true) - $start < 5);

echo "clear() drops leased locks left without an owner\n";
echo inChild(function () use ($pool) {
    return 'child lock=' . var_export($pool->lock('abandoned', 2), true);
}), "\n";
var_dump($pool->lock('abandoned', 0));
var_dump($pool->clear());
var_dump($pool->lock('abandoned', 0));
var_dump($pool->store('after', 'ok'));

?>
--EXPECT--
leased lock count per segment
acquired=32 unleased=true
lock key byte budget
first=true second=false unleased=true
lease clamped to user_cache.lock_lease_max
bool(true)
taken over
bool(true)
clear() drops leased locks left without an owner
child lock=true
bool(false)
bool(true)
bool(true)
bool(true)
