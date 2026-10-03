--TEST--
UserCache\Cache: an initialized lazy proxy is stored like serialize() stores it, while uninitialized lazy objects, including one inserted by a serialization hook, are rejected without running their initializer
--INI--
user_cache.enable=1
user_cache.enable_cli=1
--FILE--
<?php
class Point
{
    public int $x = 0;
}

$class = new ReflectionClass(Point::class);
$cache = UserCache\Cache::getPool('lazy-proxy-initialized');

$proxy = $class->newLazyProxy(function (): Point {
    $point = new Point;
    $point->x = 7;

    return $point;
});
try {
    $cache->store('proxy', $proxy);
} catch (TypeError $e) {
    echo 'uninitialized proxy: ', $e->getMessage(), "\n";
}

var_dump($proxy->x);
var_dump($cache->store('proxy', $proxy));
var_dump(serialize($cache->fetch('proxy')) === serialize(unserialize(serialize($proxy))));
var_dump($class->isUninitializedLazyObject($cache->fetch('proxy')), $cache->fetch('proxy')->x);

$nested = ['proxy' => $proxy, 'again' => $proxy];
var_dump($cache->store('nested', $nested));
$fetched = $cache->fetch('nested');
var_dump($fetched['proxy'] === $fetched['again'], $fetched['proxy']->x);

class Ghost
{
    public $v = 0;
}

class InsertsGhost
{
    public $holder;

    public function __serialize(): array
    {
        $this->holder->ghost = (new ReflectionClass(Ghost::class))->newLazyGhost(function (Ghost $ghost): void {
            echo "initializer ran\n";
            $ghost->v = 42;
        });

        return [];
    }

    public function __unserialize(array $data): void
    {
    }
}

function store_hook_inserts_lazy_object(): void
{
    $holder = new stdClass;
    $holder->ghost = null;
    $hook = new InsertsGhost;
    $hook->holder = $holder;

    $cache = UserCache\Cache::getPool('store-hook-inserts-lazy-object');
    try {
        var_dump($cache->store('k', [$holder, $hook]));
    } catch (TypeError $e) {
        echo get_class($e), ': ', $e->getMessage(), "\n";
    }
    var_dump($cache->fetch('k', 'MISS'));
    var_dump((new ReflectionClass(Ghost::class))->isUninitializedLazyObject($holder->ghost));

    /* Release builds do not collect cycles at shutdown. */
    $holder = $hook = $e = null;
    gc_collect_cycles();
}

echo "\nstore hook inserts lazy object:\n";
store_hook_inserts_lazy_object();
?>
--EXPECT--
uninitialized proxy: Uninitialized lazy objects cannot be stored in the user cache
int(7)
bool(true)
bool(true)
bool(false)
int(7)
bool(true)
bool(true)
int(7)

store hook inserts lazy object:
TypeError: Uninitialized lazy objects cannot be stored in the user cache
string(4) "MISS"
bool(true)
