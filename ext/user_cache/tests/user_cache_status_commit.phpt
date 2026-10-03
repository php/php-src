--TEST--
UserCache\CacheStatus: getCommittedMemory() reports the whole segment on platforms that back the segment lazily, and getCommitFailureCount() stays 0
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Windows') die('skip Windows commits the segment as the data grows (user_cache_win32_lazy_commit)');
?>
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=8M
--FILE--
<?php
$cache = UserCache\Cache::getPool('status-commit');
$cache->store('value', str_repeat('v', 100000));
$status = UserCache\Cache::getStatus();
var_dump($status->getCommittedMemory() === $status->getSharedMemorySize(), $status->getCommitFailureCount());
?>
--EXPECT--
bool(true)
int(0)
