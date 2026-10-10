--TEST--
UserCache\Cache: mutations do not run hidden destructors, while fetched objects and cycle-collector destructors during prototype cloning or fetchMultiple() cleanup may re-enter the pool
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=16M
--FILE--
<?php
use UserCache\Cache;

/* clear() and deletePool() do not retain extra objects with user destructors. */
class ClearReentrantDtor
{
    public static bool $armed = false;

    public int $n = 1;

    public function __destruct()
    {
        if (self::$armed) {
            UserCache\Cache::getPool('clear-dtor-reentry')->clear();
        }
    }
}

class DeletePoolReentrantDtor
{
    public static bool $armed = false;

    public int $n = 2;

    public function __destruct()
    {
        if (self::$armed) {
            self::$armed = false;

            UserCache\Cache::deletePool('clear-dtor-reentry');
        }
    }
}

function seed_request_local_slot(UserCache\Cache $cache, string $key, object $value): void
{
    $cache->store($key, $value);

    $marked = $cache->fetch($key);
    unset($marked);

    $seeded = $cache->fetch($key);
    unset($seeded);
}

$cache = UserCache\Cache::getPool('clear-dtor-reentry');
$cache->clear();

seed_request_local_slot($cache, 'single', new ClearReentrantDtor());

ClearReentrantDtor::$armed = true;
var_dump($cache->clear());
ClearReentrantDtor::$armed = false;

var_dump($cache->has('single'));

foreach (['multi-1', 'multi-2', 'multi-3'] as $key) {
    seed_request_local_slot($cache, $key, new ClearReentrantDtor());
}

ClearReentrantDtor::$armed = true;
var_dump($cache->clear());
ClearReentrantDtor::$armed = false;

var_dump($cache->has('multi-1'));
var_dump($cache->has('multi-3'));

$cache = UserCache\Cache::getPool('clear-dtor-reentry');
seed_request_local_slot($cache, 'delete-pool', new DeletePoolReentrantDtor());

DeletePoolReentrantDtor::$armed = true;
var_dump(UserCache\Cache::deletePool('clear-dtor-reentry'));
DeletePoolReentrantDtor::$armed = false;

var_dump(UserCache\Cache::getPool('clear-dtor-reentry')->has('delete-pool'));

/* Only the caller's live objects may clear the pool from their destructors. */
class SlotReplaceDtor
{
    public int $n = 1;
    public static bool $armed = false;

    public function __destruct()
    {
        if (self::$armed) {
            UserCache\Cache::getPool('slot-replace')->clear();
        }
    }
}

$cache = UserCache\Cache::getPool('slot-replace');
$cache->clear();

$obj = new SlotReplaceDtor();
var_dump($cache->store('k', $obj));

$first = $cache->fetch('k');
$second = $cache->fetch('k');
var_dump($second->n);

SlotReplaceDtor::$armed = true;
var_dump($cache->store('k', str_repeat('a', 300)));
var_dump($cache->fetch('k') === str_repeat('a', 300));
unset($obj, $first, $second);
SlotReplaceDtor::$armed = false;

var_dump($cache->fetch('k'));

/* delete() does not keep an extra object with a user destructor; a live fetched object re-enters the pool once released. */
class DeleteReentrantDtor
{
    public static bool $armed = false;

    public function __destruct()
    {
        if (self::$armed) {
            self::$armed = false;
            $cache = UserCache\Cache::getPool('delete-dtor-reentry');
            $cache->clear();
            $cache->store('from-destructor', str_repeat('y', 300));
        }
    }
}

$cache = UserCache\Cache::getPool('delete-dtor-reentry');
$cache->clear();
$cache->store('object', new DeleteReentrantDtor());
$cache->fetch('object');
$cache->fetch('object');
$held = $cache->fetch('object');
DeleteReentrantDtor::$armed = true;
var_dump($cache->delete('object'));
var_dump($cache->fetch('from-destructor') === null);
unset($held);
var_dump($cache->fetch('from-destructor') === str_repeat('y', 300));
var_dump($cache->has('object'));
echo "done\n";

echo "\nrequest-local prototype gc release:\n";

class Dto
{
    public int $a = 1;
    public array $b = [1, 2];
}

class DeletingGarbage
{
    public $self;

    public function __destruct()
    {
        global $cache, $deletes;

        $deletes++;
        $cache->delete('proto');
    }
}

$cache = Cache::getPool('prototype-gc');
$deletes = 0;
$value = ['a' => new Dto(), 'b' => new Dto(), 'c' => [new Dto(), new Dto()]];

for ($round = 0; $round < 4; $round++) {
    $cache->store('proto', $value);
    $cache->fetch('proto');
    $cache->fetch('proto');

    gc_collect_cycles();
    $status = gc_status();
    for ($i = $status['roots']; $i < $status['threshold'] - 1; $i++) {
        $garbage = new DeletingGarbage();
        $garbage->self = $garbage;
        unset($garbage);
    }

    $fetched = $cache->fetch('proto');
    var_dump($fetched == $value);
    unset($fetched);
}

gc_collect_cycles();
var_dump($deletes > 0);

echo "\nfetchMultiple exception cleanup:\n";

class StoringDestructor
{
    public $first;
    public $second;

    public function __destruct()
    {
        global $cache, $armed;

        if ($armed) {
            var_dump($cache->store('from-destructor', [1, 'x']));
        }
    }
}

class ThrowingGarbage
{
    public $self;

    public function __destruct()
    {
        global $armed, $thrown;

        if ($armed && !$thrown) {
            $thrown = true;

            throw new RuntimeException('from garbage');
        }
    }
}

$cache = UserCache\Cache::getPool('fetch-multiple-exception-cleanup');
$armed = false;
$thrown = false;

$shared = new stdClass();
$value = new StoringDestructor();
$value->first = $shared;
$value->second = $shared;
var_dump($cache->store('value', $value));
unset($value, $shared);

gc_collect_cycles();
$status = gc_status();
for ($i = $status['roots']; $i < $status['threshold'] - 1; $i++) {
    $garbage = new ThrowingGarbage();
    $garbage->self = $garbage;
    unset($garbage);
}

$armed = true;
try {
    $cache->fetchMultiple(['value']);
    echo "no exception\n";
} catch (RuntimeException $e) {
    echo $e->getMessage(), "\n";
}
$armed = false;

var_dump($cache->fetch('from-destructor'));
gc_collect_cycles();
?>
--EXPECT--
bool(true)
bool(false)
bool(true)
bool(false)
bool(false)
bool(true)
bool(false)
bool(true)
int(1)
bool(true)
bool(true)
NULL
bool(true)
bool(true)
bool(true)
bool(false)
done

request-local prototype gc release:
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

fetchMultiple exception cleanup:
bool(true)
bool(true)
from garbage
array(2) {
  [0]=>
  int(1)
  [1]=>
  string(1) "x"
}
