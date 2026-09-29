--TEST--
UserCache\Cache: the remainder of a split free block joins the free block after it when the merge limit allows
--SKIPIF--
<?php
if (!PHP_DEBUG) die('skip requires a debug build (fault injection is ZEND_DEBUG-only)');
?>
--ENV--
USER_CACHE_DEBUG_SMALL_BLOCK_MERGE_LIMIT=1
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=256K
user_cache.eviction_policy=none
--FILE--
<?php
use UserCache\Cache;

$cache = Cache::getPool('merge-limit-split');
var_dump($cache->store('a', str_repeat('a', 50000)), $cache->store('b', str_repeat('b', 30000)));
for ($fillers = 0; $cache->store("fill$fillers", str_repeat('f', 1000)); $fillers++);

/* Together the two free blocks exceed the 64 KiB merge limit, so they stay apart. */
var_dump($cache->delete('a'), $cache->delete('b'));

/* Splitting the first block leaves a remainder that fits under the limit with the second one. */
var_dump($cache->store('d', str_repeat('d', 45000)));
var_dump($cache->store('e', str_repeat('e', 33000)), $cache->fetch('e') === str_repeat('e', 33000));
var_dump(Cache::getStatus()->getStoreFailureCount());
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
int(1)
