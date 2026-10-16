--TEST--
UserCache\Cache: on Windows the segment reserves user_cache.shm_size and commits memory as the data grows, so a large segment starts with a small commit, and the tail released by a delete is reset and reused intact
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== 'Windows') die('skip Windows only: POSIX memory models map the segment lazily already');
if (PHP_INT_SIZE < 8) die('skip needs a 64-bit build to reserve 2G');
?>
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=2G
user_cache.entries_hint=1024
--FILE--
<?php
$cache = UserCache\Cache::getPool('win32-lazy-commit');
var_dump($cache->store('small', str_repeat('s', 1000)));
$status = UserCache\Cache::getStatus();
var_dump($status->getAvailability()->name, $status->getSharedMemorySize() === 2 << 30);
var_dump($status->getCommittedMemory() > 0 && $status->getCommittedMemory() < 64 << 20, $status->getCommitFailureCount());

$big = str_repeat('b', 32 << 20);
var_dump($cache->store('big', $big), $cache->fetch('big') === $big);
var_dump(UserCache\Cache::getStatus()->getCommittedMemory() >= 32 << 20);

echo "reset tail:\n";
$released = str_repeat('r', 16 << 20);
var_dump($cache->store('released', $released), $cache->delete('released'));
$reused = str_repeat('u', 16 << 20);
var_dump($cache->store('reused', $reused), $cache->fetch('reused') === $reused, $cache->fetch('big') === $big);
?>
--EXPECT--
bool(true)
string(9) "Available"
bool(true)
bool(true)
int(0)
bool(true)
bool(true)
bool(true)
reset tail:
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
