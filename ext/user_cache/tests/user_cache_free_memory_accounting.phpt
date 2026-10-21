--TEST--
UserCache: free-memory accounting follows fragmentation, splitting, coalescing and bulk rollback
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=1M
user_cache.entries_hint=127
user_cache.eviction_policy=none
--FILE--
<?php
function memoryState(): array
{
    $status = UserCache\Cache::getStatus();
    if ($status->getUsedMemory() + $status->getFreeMemory() !== $status->getSharedMemorySize()
        || $status->getWastedMemory() > $status->getFreeMemory()) {
        throw new RuntimeException('Inconsistent memory totals');
    }
    return [$status->getUsedMemory(), $status->getFreeMemory(), $status->getWastedMemory()];
}

function check(bool $condition, string $message): void
{
    if (!$condition) {
        throw new RuntimeException($message);
    }
}

$cache = UserCache\Cache::getPool('free-memory-accounting');
$cache->clear();
$empty = memoryState();
$sizes = [];
foreach (['a', 'b', 'c', 'd', 'e'] as $key) {
    $before = memoryState();
    check($cache->store($key, str_repeat($key, 16 * 1024)), 'Initial store failed');
    $after = memoryState();
    $sizes[$key] = $after[0] - $before[0];
}
check(memoryState()[2] === 0, 'Tail allocation created a free-list block');

/* Separate holes become one block when their live neighbour is deleted. */
$cache->delete('b');
$cache->delete('d');
check(memoryState()[2] === $sizes['b'] + $sizes['d'], 'Separate holes miscounted');
$cache->delete('c');
$fragmented = memoryState();
check($fragmented[2] === $sizes['b'] + $sizes['c'] + $sizes['d'], 'Coalescing miscounted');
echo "fragmentation and coalescing OK\n";

/* Both values fit individually, but the second allocation fails after the
 * first commit. Rolling back must restore the old holes and unused tail. */
check(!$cache->storeMultiple([
    'rollback-first' => str_repeat('x', 400 * 1024),
    'rollback-second' => str_repeat('y', 700 * 1024),
]), 'Oversized batch unexpectedly succeeded');
check(!$cache->has('rollback-first') && !$cache->has('rollback-second'), 'Rollback left an entry');
check(memoryState() === $fragmented, 'Rollback changed free-memory accounting');
echo "rollback OK\n";

/* A smaller allocation splits the hole, and deleting it joins it again. */
check($cache->store('f', str_repeat('f', 512)), 'Hole reuse failed');
$reused = memoryState();
$allocated = $reused[0] - $fragmented[0];
check($allocated > 0 && $reused[2] === $fragmented[2] - $allocated, 'Split remainder miscounted');
$cache->delete('f');
check(memoryState() === $fragmented, 'Freeing the split block changed the totals');

/* Freeing the tail absorbs the preceding hole: it is no longer wasted memory. */
$cache->delete('e');
check(memoryState()[2] === 0, 'Tail coalescing left free-list bytes');
$cache->delete('a');
check(memoryState() === $empty, 'Freeing all blocks did not restore the empty state');
echo "split, reuse and tail reclamation OK\n";

/* Explicit key-lock storage uses the same allocator and must balance too. */
check($cache->lock('first') && $cache->lock('second'), 'Lock allocation failed');
check($cache->unlock('first'), 'First lock release failed');
check(memoryState()[2] > 0, 'Lock release did not account for the interior hole');
check($cache->unlock('second'), 'Second lock release failed');
check(memoryState() === $empty, 'Lock storage was not fully reclaimed');
echo "lock allocations OK\n";

/* Clearing many differently sized blocks visits them in hash-table order. */
for ($i = 0; $i < 48; $i++) {
    $key = 'mixed-' . $i;
    check($cache->store($key, $i % 3 === 0 ? $i : str_repeat('m', 100 + $i * 79)), 'Mixed store failed');
}
for ($i = 0; $i < 48; $i += 2) {
    $cache->delete('mixed-' . $i);
    memoryState();
}
check($cache->clear(), 'Clear failed');
check(memoryState() === $empty, 'Clear did not reclaim fragmented allocations');
echo "fragmented clear OK\n";
?>
--EXPECT--
fragmentation and coalescing OK
rollback OK
split, reuse and tail reclamation OK
lock allocations OK
fragmented clear OK
