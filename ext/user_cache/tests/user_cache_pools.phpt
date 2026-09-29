--TEST--
UserCache\Cache: pool registry, per-pool isolation of clear() and deletePool(), and status objects
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=16M
--FILE--
<?php
/* getPool() returns per-name singletons tracked by hasPool() and getPools(). */
var_dump(UserCache\Cache::hasPool('factory'));

$a = UserCache\Cache::getPool('factory');
$b = UserCache\Cache::getPool('factory');
$c = UserCache\Cache::getPool('factory-other');

var_dump($a === $b);
var_dump($a === $c);
var_dump(UserCache\Cache::hasPool('factory'));
var_dump(UserCache\Cache::hasPool('factory-unknown'));

$pools = UserCache\Cache::getPools();
ksort($pools);
var_dump(array_keys($pools));
var_dump($pools['factory'] === $a);

try {
    new UserCache\Cache();
} catch (Error $e) {
    echo get_class($e), "\n";
}

var_dump($a->store('shared-key', 'from-factory'));
var_dump($c->fetch('shared-key', 'MISS'));
var_dump($a->fetch('shared-key'));

/* clear() only affects its own pool. */
$a = UserCache\Cache::getPool('clear-scope-a');
$b = UserCache\Cache::getPool('clear-scope-b');
$a->clear();
$b->clear();

var_dump($a->store('shared-key', 'from-a'));
var_dump($a->store('a-only', 1));
var_dump($b->store('shared-key', 'from-b'));
var_dump($b->store('b-only', 2));

var_dump($a->clear());

var_dump($a->has('shared-key'));
var_dump($a->has('a-only'));
var_dump($a->getPoolStatus()->getEntryCount());

var_dump($b->fetch('shared-key'));
var_dump($b->fetch('b-only'));
var_dump($b->getPoolStatus()->getEntryCount());

/* deletePool() removes a pool and its values, leaving other pools intact. */
$a = UserCache\Cache::getPool('delete-a');
$b = UserCache\Cache::getPool('delete-b');

$a->store('k', 'value-a');
$b->store('k', 'value-b');

var_dump(UserCache\Cache::hasPool('delete-a'));
var_dump($a->fetch('k', 'MISS'));

var_dump(UserCache\Cache::deletePool('delete-a'));

var_dump(UserCache\Cache::hasPool('delete-a'));
$pools = UserCache\Cache::getPools();
ksort($pools);
var_dump(array_keys($pools));

$a2 = UserCache\Cache::getPool('delete-a');
var_dump($a2->fetch('k', 'MISS'));
var_dump($a2->has('k'));

var_dump($b->fetch('k', 'MISS'));

var_dump(UserCache\Cache::deletePool('never-created'));

/* A live Cache object stays usable after deletePool() removed its pool. */
$a = UserCache\Cache::getPool('live-ref');
$a->store('k', 'v');
var_dump(UserCache\Cache::deletePool('live-ref'));
var_dump($a->has('k'));
var_dump($a->store('again', 1));
var_dump($a->fetch('again', 'MISS'));
var_dump(UserCache\Cache::hasPool('live-ref'));
var_dump(UserCache\Cache::getPool('live-ref')->fetch('k', 'MISS'));

/* Status objects expose the global and per-pool statistics surface. */
$cache = UserCache\Cache::getPool('stats');
$other = UserCache\Cache::getPool('stats-other');
$cache->clear();
$other->clear();

$initial = UserCache\Cache::getStatus();
$initialPool = $cache->getPoolStatus();
var_dump($initial instanceof UserCache\CacheStatus);
var_dump($initialPool instanceof UserCache\CachePoolStatus);
var_dump($initialPool->getPoolName());
var_dump($initial->getAvailability()->name);
var_dump($initial->getConfiguredMemory() === 16 * 1024 * 1024);
var_dump($initial->getSharedMemorySize() > 0);
$baseCount = $initial->getEntryCount();
var_dump($initialPool->getEntryCount());

var_dump($cache->store('key', ['value' => 1]));
var_dump($other->store('key', 'other'));

$afterStore = UserCache\Cache::getStatus();
$afterStorePool = $cache->getPoolStatus();
$afterStoreOther = $other->getPoolStatus();
var_dump($afterStore->getEntryCount() - $baseCount >= 2);
var_dump($afterStore->getGraphPinSlotsInUse() >= 0);
var_dump($afterStorePool->getEntryCount());
var_dump($afterStoreOther->getEntryCount());
var_dump($afterStore->getEntryCapacity() > 0);
var_dump($afterStore->getUsedMemory() > 0);
var_dump($afterStore->getFreeMemory() > 0);
var_dump($afterStore->getWastedMemory() >= 0);
var_dump($afterStore->getTombstoneCount() >= 0);
var_dump($afterStorePool->getUsedMemory() > 0);

var_dump($cache->clear());
var_dump($other->clear());
var_dump($cache->fetch('key', 'missing'));
var_dump($other->fetch('key', 'missing'));

$afterClear = UserCache\Cache::getStatus();
$afterClearPool = $cache->getPoolStatus();
var_dump($afterClear->getEntryCount() === $baseCount);
var_dump($afterClearPool->getEntryCount());

foreach ([$afterStore, $afterStorePool] as $object) {
    try {
        serialize($object);
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
}

try {
    $afterStore->entryCount = 0;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

/* A numeric pool name is a numeric key of getPools(), like any PHP array key. */
$numeric = UserCache\Cache::getPool('123');
$pools = UserCache\Cache::getPools();
var_dump(isset($pools['123']), $pools[123] === $numeric);
?>
--EXPECT--
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
array(2) {
  [0]=>
  string(7) "factory"
  [1]=>
  string(13) "factory-other"
}
bool(true)
Error
bool(true)
string(4) "MISS"
string(12) "from-factory"
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
int(0)
string(6) "from-b"
int(2)
int(2)
bool(true)
string(7) "value-a"
bool(true)
bool(false)
array(5) {
  [0]=>
  string(13) "clear-scope-a"
  [1]=>
  string(13) "clear-scope-b"
  [2]=>
  string(8) "delete-b"
  [3]=>
  string(7) "factory"
  [4]=>
  string(13) "factory-other"
}
string(4) "MISS"
bool(false)
string(7) "value-b"
bool(true)
bool(true)
bool(false)
bool(true)
int(1)
bool(false)
string(4) "MISS"
bool(true)
bool(true)
string(5) "stats"
string(9) "Available"
bool(true)
bool(true)
int(0)
bool(true)
bool(true)
bool(true)
bool(true)
int(1)
int(1)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
string(7) "missing"
string(7) "missing"
bool(true)
int(0)
Exception: Serialization of 'UserCache\CacheStatus' is not allowed
Exception: Serialization of 'UserCache\CachePoolStatus' is not allowed
Error: Cannot create dynamic property UserCache\CacheStatus::$entryCount
bool(true)
bool(true)
