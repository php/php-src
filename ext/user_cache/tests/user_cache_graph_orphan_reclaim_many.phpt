--TEST--
UserCache\Cache: a store under memory pressure reclaims hundreds of orphaned payloads found by scanning the heap, then keeps live entries and fetches every value
--SKIPIF--
<?php
if (!PHP_DEBUG) die('skip requires a debug build (fault injection is ZEND_DEBUG-only)');
?>
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=8M
user_cache.eviction_policy=lru
--FILE--
<?php
use UserCache\Cache;

$cache = Cache::getPool('orphan-reclaim-many');
$cache->clear();
var_dump($cache->store('live', ['kept' => true]), $cache->store('live-text', str_repeat('t', 5000)));

$pad = str_repeat('o', 20000);
for ($i = 0; $i < 200; $i++) {
    $cache->store("o:$i", ['i' => $i, 'pad' => $pad . $i]);
}
$before = Cache::getStatus()->getFreeMemory();

/* Payloads released while readers are still draining are orphaned instead of freed. */
putenv('USER_CACHE_DEBUG_FORCE_GRAPH_NOT_QUIESCENT=1');
for ($i = 0; $i < 200; $i++) {
    $cache->delete("o:$i");
}
putenv('USER_CACHE_DEBUG_FORCE_GRAPH_NOT_QUIESCENT');
var_dump(Cache::getStatus()->getFreeMemory() < $before + 1000000);

$big = str_repeat('b', 6 * 1024 * 1024);
var_dump($cache->store('big', $big));
$status = Cache::getStatus();
var_dump($status->getEvictionCount(), $status->getExpungeCount());
var_dump($cache->fetch('live'), strlen($cache->fetch('live-text')), $cache->fetch('big') === $big, $cache->has('o:0'));
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
int(0)
int(0)
array(1) {
  ["kept"]=>
  bool(true)
}
int(5000)
bool(true)
bool(false)
