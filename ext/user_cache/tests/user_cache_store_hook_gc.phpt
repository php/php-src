--TEST--
UserCache\Cache: serialization hooks and destructors that run the cycle collector or suspend their Fiber while store() encodes or copies a value leave the collector usable, still collect released cycles and never confuse a freed object with a new one
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=32M
--FILE--
<?php
class Node
{
    public $items;
}

class DropsReferences
{
    public function __serialize(): array
    {
        global $node_ref, $items_ref;

        $node_ref = null;
        $items_ref = null;
        gc_collect_cycles();

        return [];
    }

    public function __unserialize(array $data): void
    {
    }
}

class Destructible
{
    public static int $destroyed = 0;
    public $self;

    public function __destruct()
    {
        self::$destroyed++;
    }
}

class EncodeHolder
{
    public $late;
}

/* Inserted into an object that was already encoded, so its hook runs while the value is copied. */
class EncodeCollects
{
    public function __serialize(): array
    {
        gc_collect_cycles();

        return ['k' => 1];
    }

    public function __unserialize(array $data): void
    {
    }
}

class EncodeSuspends
{
    public function __serialize(): array
    {
        Fiber::suspend();

        return ['k' => 1];
    }

    public function __unserialize(array $data): void
    {
    }
}

class EncodeInserts
{
    public function __construct(public EncodeHolder $holder, public string $class)
    {
    }

    public function __serialize(): array
    {
        $this->holder->late = new $this->class;

        return [];
    }

    public function __unserialize(array $data): void
    {
    }
}

class CopyGarbage
{
    public $self;

    public function __destruct()
    {
    }
}

class Collects
{
    public $late;

    public function __destruct()
    {
        echo 'Collects::__destruct collected=', gc_collect_cycles(), "\n";
    }
}

class Suspends
{
    public $late;

    public function __destruct()
    {
        Fiber::suspend('from destructor');
    }
}

class Drops
{
    public function __serialize(): array
    {
        $GLOBALS['root']->held = null;

        return [];
    }

    public function __unserialize(array $data): void
    {
    }
}

class DropsAndThrows
{
    public function __serialize(): array
    {
        $GLOBALS['root']->held->late = null;

        throw new RuntimeException('hook failed');
    }

    public function __unserialize(array $data): void
    {
    }

    public function __destruct()
    {
        echo 'DropsAndThrows::__destruct collected=', gc_collect_cycles(), "\n";
    }
}

class Inserts
{
    public function __construct(public string $class)
    {
    }

    public function __serialize(): array
    {
        $held = $GLOBALS['root']->held;
        if (is_array($held)) {
            $GLOBALS['root']->held[] = new $this->class;
        } else {
            $held->late = new $this->class;
        }

        return [];
    }

    public function __unserialize(array $data): void
    {
    }
}

class Root
{
    public $held;
    public $late;
}

class Named
{
    public function __construct(public string $name)
    {
    }
}

/* Each hook inserts the next hooked object into a container that was already encoded, so the last one first runs in the
 * last encoding attempt; the first hook frees room for the objects the later ones add. */
class Holder
{
    public $a;
    public $l2 = [];
    public $l1 = [];
    public $l0 = [];
    public $space;
    public $first;
    public $z;
}

abstract class Chain
{
    public function __unserialize(array $data): void
    {
    }
}

final class First extends Chain
{
    public function __serialize(): array
    {
        $GLOBALS['holder']->space = null;
        $GLOBALS['holder']->l0[] = new Second;

        return [];
    }
}

final class Second extends Chain
{
    public function __serialize(): array
    {
        $GLOBALS['holder']->l1[] = new Third;

        return [];
    }
}

final class Third extends Chain
{
    public function __serialize(): array
    {
        $GLOBALS['holder']->l2[] = new Last;

        return [];
    }
}

final class Last extends Chain
{
    public function __serialize(): array
    {
        $holder = $GLOBALS['holder'];
        $holder->a = null;
        $holder->z = new Named('B');

        return [];
    }
}

/* A destructor run by the cycle collector while the value is encoded frees an encoded object. */
class Garbage
{
    public $self;

    public function __destruct()
    {
        global $phase, $garbageHolder;

        if ($phase === 'encoding') {
            $garbageHolder->a = null;
            $garbageHolder->z = new Named('B');
            $phase = 'mutated';

            return;
        }

        if ($phase !== 'done') {
            $next = new Garbage;
            $next->self = $next;
        }
    }
}

class Leaf
{
    public $v = 1;
}

class EndMarker
{
    public function __serialize(): array
    {
        $GLOBALS['phase'] = 'encoding';

        return [];
    }

    public function __unserialize(array $data): void
    {
    }
}

function make_garbage(): void
{
    for ($i = 0; $i < 100; $i++) {
        $garbage = new CopyGarbage;
        $garbage->self = $garbage;
    }
}

function report(string $label): void
{
    make_garbage();
    echo $label, ': running=', var_export(gc_status()['running'], true),
        ' protected=', var_export(gc_status()['protected'], true),
        ' collected=', gc_collect_cycles(), "\n";
}

function consistent(UserCache\Cache $cache, string $key, bool $stored, object $actual): string
{
    if (!$stored) {
        return 'not stored, ' . var_export($cache->has($key), true);
    }

    return serialize($cache->fetch($key)) === serialize($actual) ? 'stored as it is' : 'stored differently';
}

function store_hook_releases_cycle(): void
{
    global $node_ref, $items_ref;

    $node = new Node;
    $node->items = [$node, new DropsReferences];
    $node_ref = $node;
    $items_ref = $node->items;
    $holder = new stdClass;
    $holder->items = &$items_ref;
    $holder->node = &$node_ref;
    $weak = WeakReference::create($node);
    unset($node);

    var_dump(UserCache\Cache::getPool('store-hook-releases-cycle')->store('k', $holder));
    unset($holder);
    gc_collect_cycles();
    var_dump($weak->get());
}

function store_hook_gc_during_encode(): void
{
    $cache = UserCache\Cache::getPool('store-hook-gc-during-encode');

    $garbage = new Destructible;
    $garbage->self = $garbage;
    unset($garbage);
    $holder = new EncodeHolder;
    $cache->store('collects', [$holder, new EncodeInserts($holder, EncodeCollects::class)]);
    var_dump(gc_status()['running'], gc_status()['protected']);
    for ($i = 0; $i < 20000; $i++) {
        $garbage = new Destructible;
        $garbage->self = $garbage;
        unset($garbage);
    }
    gc_collect_cycles();
    var_dump(Destructible::$destroyed === 20001);

    $fiber = new Fiber(function () use ($cache): void {
        $holder = new EncodeHolder;
        $cache->store('suspends', [$holder, new EncodeInserts($holder, EncodeSuspends::class)]);
    });
    $fiber->start();
    var_dump(gc_status()['protected']);
    $fiber->resume();
    var_dump(gc_status()['protected']);
}

function store_copy_release_destructor_gc(): void
{
    global $root;

    $cache = UserCache\Cache::getPool('store-copy-release-destructor-gc');

    $root = new Root;
    $root->held = new Collects;
    make_garbage();
    var_dump($cache->store('object', [$root, new Inserts(Drops::class)]));
    report('object');

    $root = new Root;
    $root->held = [new Collects, 'x' => 1];
    make_garbage();
    var_dump($cache->store('array', [$root, new Inserts(Drops::class)]));
    report('array');

    $root = new Root;
    $root->held = new Root;
    make_garbage();
    try {
        $cache->store('failing', [$root, new Inserts(DropsAndThrows::class)]);
    } catch (RuntimeException $e) {
        echo $e->getMessage(), "\n";
    }
    report('failing');

    $root = new Root;
    $root->held = new Suspends;
    $fiber = new Fiber(fn () => $cache->store('fiber', [$root, new Inserts(Drops::class)]));
    var_dump($fiber->start());
    report('suspended');
    $fiber->resume();
    var_dump($fiber->getReturn());
    report('resumed');
}

function store_encoder_object_reuse(): void
{
    global $holder, $phase, $garbageHolder;

    $cache = UserCache\Cache::getPool('store-encoder-object-reuse');

    $holder = new Holder;
    $holder->a = new Named('A');
    $holder->space = str_repeat('x', 20000);
    $holder->first = new First;
    echo 'hook in the last attempt: ', consistent($cache, 'hooks', $cache->store('hooks', $holder), $holder), "\n";

    $phase = 'building';
    $garbageHolder = new stdClass;
    $garbageHolder->a = new Named('A');
    $garbageHolder->mid = [];
    for ($i = 0; $i < 40000; $i++) {
        $garbageHolder->mid[] = [new Leaf];
    }
    $garbageHolder->z = null;
    $garbageHolder->end = new EndMarker;
    $garbage = new Garbage;
    $garbage->self = $garbage;
    unset($garbage);
    $stored = $cache->store('gc', $garbageHolder);
    $phase = 'done';
    $fetched = $stored ? $cache->fetch('gc') : null;
    echo 'collector during encoding: ', $fetched === null || $fetched->a === null || $fetched->a !== $fetched->z ? 'no aliasing' : 'aliased', "\n";

    /* Release builds do not collect cycles at shutdown. */
    gc_collect_cycles();
}

gc_collect_cycles();
echo "store hook releases cycle:\n";
store_hook_releases_cycle();

gc_collect_cycles();
echo "\nstore hook gc during encode:\n";
store_hook_gc_during_encode();

gc_collect_cycles();
echo "\nstore copy release destructor gc:\n";
store_copy_release_destructor_gc();

gc_collect_cycles();
echo "\nstore encoder object reuse:\n";
store_encoder_object_reuse();
?>
--EXPECT--
store hook releases cycle:
bool(true)
NULL

store hook gc during encode:
bool(false)
bool(false)
bool(true)
bool(false)
bool(false)

store copy release destructor gc:
Collects::__destruct collected=100
bool(true)
object: running=false protected=false collected=100
Collects::__destruct collected=100
bool(true)
array: running=false protected=false collected=100
DropsAndThrows::__destruct collected=100
hook failed
failing: running=false protected=false collected=100
string(15) "from destructor"
suspended: running=false protected=false collected=100
bool(true)
resumed: running=false protected=false collected=100

store encoder object reuse:
hook in the last attempt: not stored, false
collector during encoding: no aliasing
