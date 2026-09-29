--TEST--
UserCache\Cache: a store under memory pressure uses the space of reclaimed orphaned payloads before evicting live entries
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

$cache = Cache::getPool('orphan-store-pressure');
$cache->clear();
var_dump($cache->store('live', 'kept'));

$pad = str_repeat('o', 150000);
for ($i = 0; $i < 40; $i++) {
    $cache->store("o:$i", ['i' => $i, 'pad' => $pad . $i]);
}

/* Payloads released while readers are still draining are orphaned instead of freed. */
putenv('USER_CACHE_DEBUG_FORCE_GRAPH_NOT_QUIESCENT=1');
for ($i = 0; $i < 40; $i++) {
    $cache->delete("o:$i");
}
putenv('USER_CACHE_DEBUG_FORCE_GRAPH_NOT_QUIESCENT');

$big = str_repeat('b', 4 * 1024 * 1024);
var_dump($cache->store('big', $big));
$status = Cache::getStatus();
var_dump($status->getEvictionCount(), $status->getExpungeCount());
var_dump($cache->fetch('live'), $cache->fetch('big') === $big);
?>
--EXPECT--
bool(true)
bool(true)
int(0)
int(0)
string(4) "kept"
bool(true)
