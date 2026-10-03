--TEST--
UserCache\Cache: free blocks that exceed the merge limit together stay linked, so freeing the tail returns all of them
--SKIPIF--
<?php
if (!PHP_DEBUG) die('skip requires a debug build (fault injection is ZEND_DEBUG-only)');
?>
--ENV--
USER_CACHE_DEBUG_SMALL_BLOCK_MERGE_LIMIT=1
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=2M
user_cache.entries_hint=100
--FILE--
<?php
use UserCache\Cache;

$cache = Cache::getPool('merge-limit');
$big = str_repeat('B', 1500 * 1024);

echo "refused merge with the previous block:\n";
$cache->clear();
$cache->store('q', str_repeat('q', 900 * 1024));
$cache->store('x', str_repeat('x', 30 * 1024));
$cache->store('y', 'tail');
var_dump($cache->delete('q'), $cache->delete('x'), $cache->delete('y'));
var_dump($cache->store('big', $big), $cache->has('big'));

echo "merge into a block whose own previous merge was refused:\n";
$cache->clear();
$cache->store('q', str_repeat('q', 900 * 1024));
$cache->store('p', str_repeat('p', 30 * 1024));
$cache->store('b', str_repeat('b', 10 * 1024));
$cache->store('y', 'tail');
var_dump($cache->delete('q'), $cache->delete('p'), $cache->delete('b'), $cache->delete('y'));
var_dump($cache->store('big', $big), $cache->has('big'));

echo "refused merge with the next block:\n";
$cache->clear();
$cache->store('a', str_repeat('a', 30 * 1024));
$cache->store('n', str_repeat('n', 900 * 1024));
$cache->store('y', 'tail');
var_dump($cache->delete('n'), $cache->delete('a'), $cache->delete('y'));
var_dump($cache->store('big', $big), $cache->has('big'));
var_dump(Cache::getStatus()->getStoreFailureCount());
?>
--EXPECT--
refused merge with the previous block:
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
merge into a block whose own previous merge was refused:
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
refused merge with the next block:
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
int(0)
