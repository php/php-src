--TEST--
UserCache bounds key records per request by the entry capacity, also for pools deleted while in use, drops the older half at the limit and never drops records held by an active, suspended or fetchMultiple() call
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=16M
--FILE--
<?php
use UserCache\Cache;

function touch_keys(UserCache\Cache $cache, string $prefix, int $count): void
{
    for ($i = 0; $i < $count; $i++) {
        $cache->has("$prefix-$i");
    }
}

function key_records_deleted_pool(bool $measured): void
{
    $limit = max(16384, UserCache\Cache::getStatus()->getEntryCapacity());

    $registered = UserCache\Cache::getPool('records-registered');
    $base = memory_get_usage();
    touch_keys($registered, 'key', 4 * $limit);
    $bounded = memory_get_usage() - $base;

    /* The object keeps working after its pool is deleted and is no longer registered. */
    $deleted = UserCache\Cache::getPool('records-deleted');
    var_dump($deleted->store('x', 1), UserCache\Cache::deletePool('records-deleted'));
    $base = memory_get_usage();
    touch_keys($deleted, 'key', 4 * $limit);
    var_dump(!$measured || memory_get_usage() - $base < 2 * $bounded);
    var_dump($registered->has('key-0') === false, $deleted->store('y', 2), $deleted->fetch('y'));

    UserCache\Cache::deletePool('records-registered');
}

class TouchesManyKeys
{
    public $n = 1;

    public function __wakeup(): void
    {
        static $nested = false;

        if ($nested) {
            return;
        }

        $nested = true;
        $cache = UserCache\Cache::getPool('records-limit');

        touch_keys($cache, 'wakeup', $GLOBALS['limit']);

        $cache->fetch('outer');
    }
}

class SuspendsOnWakeup
{
    public $n = 1;

    public function __wakeup(): void
    {
        Fiber::suspend();
    }
}

/* Without the Zend memory manager memory_get_usage() reports 0. */
$measured = getenv('USE_ZEND_ALLOC') !== '0';

echo "deleted pool:\n";
key_records_deleted_pool($measured);

echo "\nrecord limit:\n";
$cache = UserCache\Cache::getPool('records-limit');
/* The limit is the entry capacity, but at least 16384 records. */
$limit = max(16384, UserCache\Cache::getStatus()->getEntryCapacity());

$base = memory_get_usage();
touch_keys($cache, 'warm', $limit - 1);
$full = memory_get_usage() - $base;
touch_keys($cache, 'cross', 2);
$trimmed = memory_get_usage() - $base;
var_dump(!$measured || ($trimmed > $full / 3 && $trimmed < $full * 2 / 3));
touch_keys($cache, 'key', 3 * $limit);
var_dump(memory_get_usage() - $base <= $full * 1.1);

/* The limit also applies while code runs inside a fiber. */
$fiber = new Fiber(function () use ($cache, $limit): int {
    $base = memory_get_usage();
    touch_keys($cache, 'fiber-key', 3 * $limit);
    return memory_get_usage() - $base;
});
$fiber->start();
var_dump($fiber->getReturn() <= $full * 1.1);

/* Records of many pools are bounded together. */
$base = memory_get_usage();
for ($p = 0; $p < 64; $p++) {
    touch_keys(UserCache\Cache::getPool("records-limit-$p"), 'key', intdiv(3 * $limit, 64));
}
var_dump(memory_get_usage() - $base <= $full * 1.1);

$cache->store('outer', new TouchesManyKeys);
var_dump($cache->fetch('outer') instanceof TouchesManyKeys);

var_dump($cache->remember('remembered', function () use ($cache, $limit) {
    touch_keys($cache, 'callback', $limit);
    $cache->store('callback-last', 4999);

    return 'value';
}));
var_dump($cache->fetch('remembered'), $cache->fetch('callback-last'));

var_dump($cache->fetchMultiple(['outer', 'remembered', 'missing'], 'default')['remembered']);

/* Restore hooks cannot suspend the fiber of a fetch; a fiber suspended inside a callback still holds the record of its call. */
$cache->store('suspends', new SuspendsOnWakeup);
$fiber = new Fiber(function () use ($cache): array {
    try {
        $woken = $cache->fetch('suspends');
    } catch (FiberError $e) {
        $woken = $e->getMessage();
    }

    return [
        $woken,
        $cache->remember('in-fiber', function () {
            Fiber::suspend();

            return 'from fiber';
        }),
    ];
});
$fiber->start();
touch_keys($cache, 'while-suspended', $limit);
$fiber->resume();
[$woken, $remembered] = $fiber->getReturn();
var_dump($woken, $remembered, $cache->fetch('in-fiber'));

UserCache\Cache::deletePool('records-limit');
for ($p = 0; $p < 64; $p++) {
    UserCache\Cache::deletePool("records-limit-$p");
}
unset($cache, $fiber);

function key_records_held_by_calls(): void
{
    $pool = Cache::getPool('key-records-held-by-calls');
    $limit = max(16384, Cache::getStatus()->getEntryCapacity());
    $bound = 8 << 20;

    $fiber = new Fiber(fn () => $pool->remember('slow', function () {
        Fiber::suspend();

        return 'computed';
    }));
    $fiber->start();
    $base = memory_get_usage();
    for ($i = 0; $i < 3 * $limit; $i++) {
        $pool->has("suspended-$i");
    }
    echo 'bounded while a call is suspended: ', var_export(memory_get_usage() - $base < $bound, true), "\n";
    $fiber->resume();
    var_dump($fiber->getReturn(), $pool->fetch('slow'));

    $base = memory_get_usage();
    $peak = 0;
    $dependencies = 50;
    for ($i = 0; $i < intdiv(3 * $limit, $dependencies + 1); $i++) {
        $pool->remember("item-$i", function () use ($pool, $dependencies, $i) {
            for ($j = 0; $j < $dependencies; $j++) {
                $pool->has("dependency-$i-$j");
            }

            return $i;
        });
        $peak = max($peak, memory_get_usage() - $base);
    }
    echo 'bounded while records are created inside callbacks: ', var_export($peak < $bound, true), "\n";

    Cache::deletePool('key-records-held-by-calls');
}

echo "\nheld by calls:\n";
key_records_held_by_calls();

echo "\nfetchMultiple reference keys:\n";

final class Renames
{
    public $v = 1;

    public function __wakeup(): void
    {
        global $key, $other, $renamed;

        if ($renamed) {
            return;
        }

        $renamed = true;
        $key = 'renamed-by-hook';
        $limit = max(16384, Cache::getStatus()->getEntryCapacity());
        for ($i = 0; $i < $limit + 10; $i++) {
            $other->has("other-$i");
        }
    }
}

$renamed = false;
$pool = Cache::getPool('fetch-multiple-reference-keys');
$other = Cache::getPool('fetch-multiple-reference-keys-other');
$pool->store('a', new Renames);
$pool->store('b', [new ArrayObject([1, 2]), 'x']);
$key = 'b';
$keys = ['a', &$key];
$fetched = $pool->fetchMultiple($keys);
var_dump(array_keys($fetched), $fetched['b'][0]->getArrayCopy(), $fetched['b'][1], $key);

echo "\ntrim destructor throws:\n";

final class TrimThrower
{
    public $self;

    public function __destruct()
    {
        throw new Exception('from destructor during trim');
    }
}

function key_records_trim_destructor_throws(): void
{
    $cache = Cache::getPool('trim-destructor-throws');
    $other = Cache::getPool('trim-destructor-throws-other');
    $limit = max(16384, Cache::getStatus()->getEntryCapacity());

    /* The pinned filler exhausts the request's pin budget, so the next array is a private copy held by its record. */
    $cache->store('filler', range(1, 300000));
    $cache->store('copied', range(1, 300000));
    $filler = $cache->fetch('filler');
    $copied = $cache->fetch('copied');

    $keys = [];
    for ($i = 0; $i < $limit + 100; $i++) {
        $keys[] = "k$i";
    }
    $other->fetchMultiple($keys);
    unset($keys);

    gc_collect_cycles();

    $thrower = new TrimThrower();
    $thrower->self = $thrower;
    unset($thrower);

    $roots = [];
    $status = gc_status();
    for ($i = $status['threshold'] - $status['roots'] - 1; $i > 0; $i--) {
        $roots[$i] = [$i];
        $root = $roots[$i];
        unset($root);
    }

    try {
        var_dump($cache->lock('trim-lock'));
    } catch (Exception $e) {
        echo $e->getMessage(), "\n";
    }

    var_dump($cache->unlock('trim-lock'), count($copied), count($filler));
}

key_records_trim_destructor_throws();
?>
--EXPECT--
deleted pool:
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
int(2)

record limit:
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
string(5) "value"
string(5) "value"
int(4999)
string(5) "value"
string(49) "Cannot switch fibers in current execution context"
string(10) "from fiber"
string(10) "from fiber"

held by calls:
bounded while a call is suspended: true
string(8) "computed"
string(8) "computed"
bounded while records are created inside callbacks: true

fetchMultiple reference keys:
array(2) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "b"
}
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
string(1) "x"
string(15) "renamed-by-hook"

trim destructor throws:
from destructor during trim
bool(false)
int(300000)
int(300000)
