--TEST--
UserCache pool status snapshots stay independent, follow entry mutations, account used memory per pool and let cycle-collector destructors use the cache while a snapshot is refreshed
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=8M
--FILE--
<?php
use UserCache\Cache;

function keysOf($status): array {
    $keys = $status->getEntryKeys();
    sort($keys);
    return $keys;
}

$cache = Cache::getPool('status-snapshots');
$other = Cache::getPool('status-snapshots-other');
$cache->clear();
$other->clear();
$cache->store('a', 1);
$first = $cache->getPoolStatus();
$second = $cache->getPoolStatus();
var_dump($first !== $second, keysOf($first), keysOf($second));
$modified = $second->getEntryKeys();
$modified[0] = 'changed';
$modified[] = &$modified;
var_dump(keysOf($cache->getPoolStatus()));
/* Release builds do not collect cycles at shutdown. */
$modified = null;

$cache->store('a', str_repeat('a', 8192));
$cache->storeMultiple(['b' => 2, 'c' => 3]);
$updated = $cache->getPoolStatus();
var_dump($updated->getEntryCount(), keysOf($updated));
var_dump($updated->getUsedMemory() > $first->getUsedMemory());
var_dump($first->getEntryCount(), keysOf($first));
$other->store('unrelated', 42);
var_dump($cache->getPoolStatus()->getUsedMemory() === $updated->getUsedMemory());

$cache->deleteMultiple(['a', 'b']);
var_dump(keysOf($cache->getPoolStatus()));
$cache->store('ttl', 'expires', 1);
$beforeExpiry = $cache->getPoolStatus();
sleep(2);
var_dump($cache->has('ttl'));
// Status accounts for physically resident entries, including unreclaimed TTLs.
var_dump(keysOf($cache->getPoolStatus()) === keysOf($beforeExpiry));
$cache->delete('ttl');
var_dump(keysOf($cache->getPoolStatus()));

// Churn triggers tombstone removal and slot relocation.
for ($i = 0; $i < 512; $i++) {
    $cache->store("churn:$i", $i);
}
$full = $cache->getPoolStatus();
for ($i = 0; $i < 512; $i++) {
    $cache->delete("churn:$i");
}
var_dump($full->getEntryCount(), keysOf($cache->getPoolStatus()));
$cache->clear();
$empty = $cache->getPoolStatus();
var_dump($empty->getEntryCount(), $empty->getEntryKeys(), $empty->getUsedMemory());

Cache::deletePool('status-snapshots');
unset($cache);
$replacement = Cache::getPool('status-snapshots');
$replacement->store('replacement', true);
var_dump(keysOf($replacement->getPoolStatus()), keysOf($first));

$left = Cache::getPool('status-memory-a');
$right = Cache::getPool('status-memory-b');
foreach ([$left, $right] as $pool) {
    $pool->store('int', 123);
    $pool->store('str', str_repeat('x', 100));
}
$before = $left->getPoolStatus()->getUsedMemory();
var_dump($right->getPoolStatus()->getUsedMemory() === $before);
var_dump($left->store('large', str_repeat('y', 4096)));
var_dump($left->getPoolStatus()->getUsedMemory() > $before);
var_dump($left->delete('large'));
var_dump($left->getPoolStatus()->getUsedMemory() === $before);

$shrink = Cache::getPool('status-shrink');
$shrink->store('value', 1);
$shrink->store('value', str_repeat('z', 4000));
$large = $shrink->getPoolStatus()->getUsedMemory();
$shrink->store('value', 'z');
echo "in-place shrink is accounted: ";
var_dump($shrink->getPoolStatus()->getUsedMemory() < $large - 3000);

echo "\npool status destructor reentry:\n";

class Reenters
{
    public $self;

    public function __destruct()
    {
        global $cache;

        var_dump($cache->store('from-destructor', 1));
    }
}

unset($first, $second);
gc_collect_cycles();

$cache = UserCache\Cache::getPool('pool-status-dtor-reentry');
$cache->clear();
$cache->store('k1', 'v1');

/* The first status object shares the snapshot's key list. */
$first = $cache->getPoolStatus();
$cache->store('k2', 'v2');

$obj = new Reenters;
$obj->self = $obj;
unset($obj);

/* Fill the root buffer so that dropping the old key list runs the GC. */
$keep = [];
$threshold = gc_status()['threshold'];
for ($i = 0; gc_status()['roots'] < $threshold - 1; $i++) {
    $arr = [$i];
    $keep[] = $arr;
    unset($arr);
}

$second = $cache->getPoolStatus();
var_dump($first->getEntryCount(), $second->getEntryCount(), $cache->fetch('from-destructor'));
?>
--EXPECT--
bool(true)
array(1) {
  [0]=>
  string(1) "a"
}
array(1) {
  [0]=>
  string(1) "a"
}
array(1) {
  [0]=>
  string(1) "a"
}
int(3)
array(3) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "b"
  [2]=>
  string(1) "c"
}
bool(true)
int(1)
array(1) {
  [0]=>
  string(1) "a"
}
bool(true)
array(1) {
  [0]=>
  string(1) "c"
}
bool(false)
bool(true)
array(1) {
  [0]=>
  string(1) "c"
}
int(513)
array(1) {
  [0]=>
  string(1) "c"
}
int(0)
array(0) {
}
int(0)
array(1) {
  [0]=>
  string(11) "replacement"
}
array(1) {
  [0]=>
  string(1) "a"
}
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
in-place shrink is accounted: bool(true)

pool status destructor reentry:
bool(true)
int(1)
int(2)
int(1)
