--TEST--
UserCache\Cache: expired entries are reclaimed across forked workers, read-only shutdown and write traffic
--EXTENSIONS--
pcntl
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=1M
user_cache.entries_hint=127
--FILE--
<?php
require __DIR__ . '/user_cache_clock.inc';

/* Parent write sweep reclaims an expiring entry stored by a forked child */
$cache = UserCache\Cache::getPool('ttl-fork');
$cache->clear();
$cache->store('parent', 1);
$pid = pcntl_fork();
if ($pid === 0) {
    exit($cache->store('child', 2, 1) ? 0 : 1);
}
pcntl_waitpid($pid, $status);
var_dump(pcntl_wexitstatus($status));
var_dump($cache->getPoolStatus()->getEntryCount());
sleep(2);
for ($i = 0; $i < 128; $i++) {
    $cache->store('parent', $i);
}
var_dump($cache->getPoolStatus()->getEntryKeys());

/* Expired entries observed by read misses are reclaimed by the next single store */
$cache = UserCache\Cache::getPool('expired-read-reclaim');
$cache->clear();
$start = hrtime();
for ($i = 0; $i < 80; $i++) {
    $cache->store('expiring-' . $i, str_repeat('v', 64), 1);
}
$cache->store('persistent', 'kept');
/* A 1s TTL lasts at least a second; only a slower loop lets the write sweep reclaim entries. */
var_dump(seconds_since($start) >= 1 || $cache->getPoolStatus()->getEntryCount() === 81);
sleep(2);
for ($i = 0; $i < 80; $i++) {
    if ($cache->fetch('expiring-' . $i, null) !== null) {
        exit('unexpected hit');
    }
}
$cache->store('trigger', 'mutation');
var_dump($cache->getPoolStatus()->getEntryCount());
var_dump($cache->fetch('persistent'));
var_dump($cache->fetch('trigger'));

/* Read-only request shutdown and write traffic alone return memory to the baseline */
const KEYS = 100;
$cache = UserCache\Cache::getPool('expired-reclaim');
$cache->clear();

function free_memory(): int {
    return UserCache\Cache::getStatus()->getFreeMemory();
}

function seed(UserCache\Cache $cache): void {
    $pad = str_repeat('x', 2000);
    for ($i = 0; $i < KEYS; $i++) {
        $cache->store("e:$i", $pad, 1);
    }
}

var_dump(UserCache\Cache::getStatus()->getEntryCapacity() <= 4096);
$baseline = free_memory();

seed($cache);
sleep(2);
var_dump(free_memory() < $baseline);
$pid = pcntl_fork();
if ($pid === 0) {
    for ($i = 0; $i < KEYS; $i++) {
        $cache->fetch("e:$i");
    }
    exit(0);
}
pcntl_waitpid($pid, $status);
var_dump(free_memory() === $baseline);

seed($cache);
sleep(2);
for ($w = 0; $w < 70; $w++) {
    $cache->store('w', $w);
}
$cache->delete('w');
var_dump(free_memory() === $baseline);
?>
--EXPECT--
int(0)
int(2)
array(1) {
  [0]=>
  string(6) "parent"
}
bool(true)
int(2)
string(4) "kept"
string(8) "mutation"
bool(true)
bool(true)
bool(true)
bool(true)
