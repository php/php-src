--TEST--
UserCache\Cache: on Windows a commit that fails while the data grows is counted and makes stores behave as if the cache were full; the cache stays available
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== 'Windows') die('skip Windows only: the commit fault point is in the win32 segment');
if (!PHP_DEBUG) die('skip requires a debug build (fault injection is ZEND_DEBUG-only)');
?>
--ENV--
USER_CACHE_DEBUG_FAIL_WIN32_COMMIT=1
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=64M
user_cache.entries_hint=1024
user_cache.eviction_policy=none
--FILE--
<?php
$cache = UserCache\Cache::getPool('win32-commit-failure');
var_dump($cache->store('small', 'fits in the committed prefix chunk'));
var_dump($cache->store('big', str_repeat('b', 8 << 20)), $cache->has('big'));
$status = UserCache\Cache::getStatus();
var_dump($status->getAvailability()->name, $status->getCommitFailureCount() > 0, $status->getCommittedMemory() < $status->getSharedMemorySize());
var_dump($cache->fetch('small'));
?>
--EXPECT--
bool(true)
bool(false)
bool(false)
string(9) "Available"
bool(true)
bool(true)
string(34) "fits in the committed prefix chunk"
