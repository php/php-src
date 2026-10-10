--TEST--
UserCache\Cache: graph payloads released while readers are still draining are orphaned and reclaimed by the next sweep, clear() or deletePool()
--SKIPIF--
<?php
if (!PHP_DEBUG) die('skip requires a debug build (fault injection is ZEND_DEBUG-only)');
?>
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=8M
--FILE--
<?php
function free_memory(): int {
    return UserCache\Cache::getStatus()->getFreeMemory();
}

function seed(UserCache\Cache $cache, int $count): void {
    $pad = str_repeat('o', 3000);
    for ($i = 0; $i < $count; $i++) {
        $cache->store("o:$i", ['i' => $i, 'pad' => $pad]);
    }
}

/* More payloads than the orphan list holds are freed by the periodic write sweep. */
$cache = UserCache\Cache::getPool('orphan-sweep');
$cache->clear();
$cache->store('tick', 0);
for ($i = 20; $i < 40; $i++) {
    $cache->store("o:$i", $i);
}
$baseline = free_memory();

seed($cache, 40);
putenv('USER_CACHE_DEBUG_FORCE_GRAPH_NOT_QUIESCENT=1');
for ($i = 0; $i < 20; $i++) {
    $cache->delete("o:$i");
}
for ($i = 20; $i < 40; $i++) {
    $cache->store("o:$i", $i);
}
putenv('USER_CACHE_DEBUG_FORCE_GRAPH_NOT_QUIESCENT');

echo "orphaned: ";
var_dump(free_memory() < $baseline);

for ($i = 1; $i <= 64; $i++) {
    $cache->store('tick', $i);
}
echo "reclaimed by the sweep: ";
var_dump(free_memory() === $baseline);

/* Pool clear() and deletePool() reclaim orphans of any pool immediately once readers have drained. */
$cache = UserCache\Cache::getPool('orphan-clear');
$cache->clear();
$baseline = free_memory();

seed($cache, 5);
putenv('USER_CACHE_DEBUG_FORCE_GRAPH_NOT_QUIESCENT=1');
var_dump($cache->clear());
putenv('USER_CACHE_DEBUG_FORCE_GRAPH_NOT_QUIESCENT');

echo "orphaned by clear() while draining: ";
var_dump($cache->has('o:0'), free_memory() < $baseline);

var_dump(UserCache\Cache::getPool('orphan-unrelated')->clear());
echo "reclaimed by the next clear(): ";
var_dump(free_memory() === $baseline);

seed($cache, 5);
putenv('USER_CACHE_DEBUG_FORCE_GRAPH_NOT_QUIESCENT=1');
for ($i = 0; $i < 5; $i++) {
    $cache->delete("o:$i");
}
putenv('USER_CACHE_DEBUG_FORCE_GRAPH_NOT_QUIESCENT');

echo "orphaned by delete() while draining: ";
var_dump(free_memory() < $baseline);
var_dump(UserCache\Cache::deletePool('orphan-unrelated'));
echo "reclaimed by deletePool(): ";
var_dump(free_memory() === $baseline);
?>
--EXPECT--
orphaned: bool(true)
reclaimed by the sweep: bool(true)
bool(true)
orphaned by clear() while draining: bool(false)
bool(true)
bool(true)
reclaimed by the next clear(): bool(true)
orphaned by delete() while draining: bool(true)
bool(true)
reclaimed by deletePool(): bool(true)
