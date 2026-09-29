--TEST--
UserCache\Cache: restore hooks observe completed graphs, isolate nested fetches, failures, Fiber switches and native unserialize() contexts, may store, delete or clear the key being fetched, and release interrupted decodes on a fatal error
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=16M
--FILE--
<?php
class RestoreParent
{
    public object $child;
    public string $name;
}

class RestoreWakeup
{
    public static array $events = [];
    public RestoreParent $parent;
    public bool $nested = false;

    public function __wakeup(): void
    {
        self::$events[] = 'wakeup:' . $this->parent->name;
        if ($this->nested) {
            UserCache\Cache::getPool('deferred-restore')->fetch('nested');
            self::$events[] = 'returned:' . $this->parent->name;
        }
    }
}

class RestoreUnserialize
{
    public RestoreParent $parent;

    public function __serialize(): array
    {
        return ['parent' => $this->parent];
    }

    public function __unserialize(array $state): void
    {
        $this->parent = $state['parent'];
        RestoreWakeup::$events[] = 'unserialize:' . $this->parent->name;
    }
}

function graph(object $child, string $name): RestoreParent
{
    $parent = new RestoreParent;
    $parent->child = $child;
    $parent->name = $name;
    $child->parent = $parent;
    return $parent;
}

$cache = UserCache\Cache::getPool('deferred-restore');
$cache->store('nested', graph(new RestoreUnserialize, 'nested'));
$wakeup = new RestoreWakeup;
$wakeup->nested = true;
$value = [graph($wakeup, 'outer'), graph(new RestoreUnserialize, 'sibling')];
$cache->store('value', $value);

RestoreWakeup::$events = [];
$native = unserialize(serialize($value));
$expected = RestoreWakeup::$events;
RestoreWakeup::$events = [];
$result = $cache->fetch('value');
var_dump(RestoreWakeup::$events === $expected);
echo implode(',', RestoreWakeup::$events), "\n";
var_dump($result[0]->child->parent === $result[0]);
var_dump($result[1]->child->parent === $result[1]);

class ThrowingRestore
{
    public static array $events = [];
    public static array $destroyed = [];
    public static bool $throw = false;
    public function __construct(public string $name) {}
    public function __wakeup(): void
    {
        self::$events[] = $this->name;
        if (self::$throw && $this->name === 'second') {
            throw new RuntimeException('restore failed');
        }
    }
    public function __destruct()
    {
        self::$destroyed[] = $this->name;
    }
}

$cache->store('throwing', [new ThrowingRestore('first'), new ThrowingRestore('second'), new ThrowingRestore('third')]);
ThrowingRestore::$throw = true;
ThrowingRestore::$destroyed = [];
try {
    $cache->fetch('throwing');
} catch (RuntimeException $exception) {
    echo $exception->getMessage(), "\n";
}
echo implode(',', ThrowingRestore::$events), "\n";
echo 'destructors:', implode(',', ThrowingRestore::$destroyed), "\n";
ThrowingRestore::$throw = false;
ThrowingRestore::$events = [];
$result = $cache->fetch('throwing');
echo implode(',', ThrowingRestore::$events), "\n";
var_dump(count($result));
RestoreWakeup::$events = [];
$cache->fetch('value');
var_dump(RestoreWakeup::$events === $expected);

class CatchingRestore
{
    public function __wakeup(): void
    {
        try {
            UserCache\Cache::getPool('deferred-restore')->fetch('throwing');
        } catch (RuntimeException $exception) {
            echo 'nested: ', $exception->getMessage(), "\n";
        }
    }
}

$cache->store('catching', [new CatchingRestore, graph(new RestoreUnserialize, 'after-catch')]);
ThrowingRestore::$throw = true;
ThrowingRestore::$events = [];
RestoreWakeup::$events = [];
$result = $cache->fetch('catching');
echo implode(',', ThrowingRestore::$events), "\n";
echo implode(',', RestoreWakeup::$events), "\n";
var_dump($result[1]->child->parent === $result[1]);

/* Release builds do not collect cycles at shutdown. */
$native = $result = $value = $wakeup = null;
gc_collect_cycles();

class Node
{
    public $tag;
    public $peer;

    public function __wakeup(): void
    {
        if (Fiber::getCurrent() !== null) {
            Fiber::suspend($this->tag);
        }
    }
}

function fetch_restore_hook_fiber_switch(): void
{
    $cache = UserCache\Cache::getPool('fetch-restore-hook-fiber-switch');
    foreach (['a', 'b'] as $key) {
        $node = new Node;
        $node->tag = "$key-node";
        $node->peer = $node;
        $cache->store($key, [$node, $node]);
    }

    $fibers = [];
    foreach (['a', 'b'] as $key) {
        $fibers[$key] = new Fiber(function () use ($cache, $key): string {
            try {
                $cache->fetch($key);

                return 'fetched';
            } catch (FiberError $e) {
                return $e->getMessage();
            }
        });
    }
    foreach ($fibers as $key => $fiber) {
        var_dump($fiber->start(), $fiber->getReturn());
    }

    $fetched = $cache->fetch('a');
    var_dump($fetched[0]->tag, $fetched[0] === $fetched[1], $fetched[0]->peer === $fetched[0]);

    /* Release builds do not collect cycles at shutdown. */
    $node = $fetched = null;
    gc_collect_cycles();
}

echo "\nfetch restore hook fiber switch:\n";
fetch_restore_hook_fiber_switch();

class Waker
{
    public $probe;

    public function __wakeup(): void
    {
        $this->probe = unserialize('a:2:{i:0;O:8:"stdClass":1:{s:3:"tag";s:5:"inner";}i:1;r:2;}');
    }
}

class Probe
{
    public $probe;

    public function __unserialize(array $data): void
    {
        $this->probe = unserialize('a:2:{i:0;O:8:"stdClass":1:{s:3:"tag";s:5:"inner";}i:1;r:2;}');
    }

    public function __serialize(): array
    {
        return [];
    }
}

class Outer implements Serializable
{
    public static array $fetched = [];

    public function serialize(): string
    {
        return '';
    }

    public function unserialize(string $data): void
    {
        $cache = UserCache\Cache::getPool('graph-restore-serialize-lock');
        self::$fetched[] = $cache->fetch('waker');
        self::$fetched[] = $cache->fetch('probe');
    }

    public function __serialize(): array
    {
        return [];
    }

    public function __unserialize(array $data): void
    {
    }
}

function graph_restore_serialize_lock(): void
{
    $cache = UserCache\Cache::getPool('graph-restore-serialize-lock');
    $cache->store('waker', new Waker());
    $cache->store('probe', new Probe());

    $outer = unserialize('a:2:{i:0;O:8:"stdClass":1:{s:3:"tag";s:5:"outer";}i:1;C:5:"Outer":0:{}}');
    var_dump($outer[0]->tag, get_class($outer[1]));
    foreach (Outer::$fetched as $fetched) {
        var_dump($fetched->probe[0]->tag, $fetched->probe[1]->tag);
    }
}

echo "\ngraph restore serialize lock:\n";
graph_restore_serialize_lock();

class WakeupWritesFetchedKey
{
    public static array $returns = [];

    public function __construct(public string $action) {}

    public function __wakeup(): void
    {
        self::$returns[] = hook_write_fetched_key($this->action);
    }
}

class UnserializeWritesFetchedKey
{
    public function __construct(public string $action) {}

    public function __serialize(): array
    {
        return ['action' => $this->action];
    }

    public function __unserialize(array $data): void
    {
        $this->action = $data['action'];
        WakeupWritesFetchedKey::$returns[] = hook_write_fetched_key($this->action);
    }
}

function hook_write_fetched_key(string $action): bool
{
    $cache = UserCache\Cache::getPool('restore-hook-writes-fetched-key');

    return match ($action) {
        'store' => $cache->store('fetched', 'stored by hook'),
        'delete' => $cache->delete('fetched'),
        'clear' => $cache->clear(),
        'deletePool' => UserCache\Cache::deletePool('restore-hook-writes-fetched-key'),
    };
}

function restore_hook_writes_fetched_key(): void
{
    $payload = str_repeat('p', 5000);
    foreach ([WakeupWritesFetchedKey::class, UnserializeWritesFetchedKey::class] as $class) {
        foreach (['store', 'delete', 'clear', 'deletePool'] as $action) {
            $cache = UserCache\Cache::getPool('restore-hook-writes-fetched-key');
            $cache->store('fetched', [new $class($action), $payload]);
            $cache->store('sibling', 'sibling');
            WakeupWritesFetchedKey::$returns = [];

            $fetched = $cache->fetch('fetched');
            $current = UserCache\Cache::getPool('restore-hook-writes-fetched-key');
            for ($i = 0; $i < 8; $i++) {
                $current->store("filler-$i", str_repeat('q', 5000));
            }
            printf(
                "%s %s: hook %s, fetched the stored value %s, then %s, sibling %s, same pool object %s\n",
                $class,
                $action,
                implode(',', array_map(fn($returned) => var_export($returned, true), WakeupWritesFetchedKey::$returns)),
                var_export($fetched[0] instanceof $class && $fetched[0]->action === $action && $fetched[1] === $payload, true),
                var_export($cache->fetch('fetched', 'MISS'), true),
                var_export($current->fetch('sibling', 'MISS'), true),
                var_export($current === $cache, true)
            );
            $current->clear();
        }
    }
}

echo "\nrestore hook writes fetched key:\n";
restore_hook_writes_fetched_key();

class BailoutInner
{
    public $peer;

    public function __wakeup(): void
    {
        eval('class GraphRestoreHookBailout {} class GraphRestoreHookBailout {}');
    }
}

class BailoutOuter
{
    public $peer;

    public function __wakeup(): void
    {
        echo "Outer::__wakeup\n";
        UserCache\Cache::getPool('graph-restore-hook-bailout')->fetch('inner');
    }
}

function graph_restore_hook_bailout(): void
{
    $cache = UserCache\Cache::getPool('graph-restore-hook-bailout');

    /* Shared objects make both decodes keep identity maps alive while their
     * restore hooks run. */
    $inner = new BailoutInner();
    $inner->peer = $inner;
    $cache->store('inner', [$inner, $inner]);

    $outer = new BailoutOuter();
    $outer->peer = $outer;
    $cache->store('outer', [$outer, $outer]);

    /* Release builds do not collect cycles at shutdown. */
    $inner = $outer = null;
    gc_collect_cycles();

    $cache->fetch('outer');
    echo "unreachable\n";
}

echo "\ngraph restore hook bailout:\n";
graph_restore_hook_bailout();
?>
--EXPECTF--
bool(true)
wakeup:outer,unserialize:nested,returned:outer,unserialize:sibling
bool(true)
bool(true)
restore failed
first,second
destructors:first
first,second,third
int(3)
bool(true)
nested: restore failed
first,second
unserialize:after-catch
bool(true)

fetch restore hook fiber switch:
NULL
string(49) "Cannot switch fibers in current execution context"
NULL
string(49) "Cannot switch fibers in current execution context"
string(6) "a-node"
bool(true)
bool(true)

graph restore serialize lock:
string(5) "outer"
string(5) "Outer"
string(5) "inner"
string(5) "inner"
string(5) "inner"
string(5) "inner"

restore hook writes fetched key:
WakeupWritesFetchedKey store: hook true, fetched the stored value true, then 'stored by hook', sibling 'sibling', same pool object true
WakeupWritesFetchedKey delete: hook true, fetched the stored value true, then 'MISS', sibling 'sibling', same pool object true
WakeupWritesFetchedKey clear: hook true, fetched the stored value true, then 'MISS', sibling 'MISS', same pool object true
WakeupWritesFetchedKey deletePool: hook true, fetched the stored value true, then 'MISS', sibling 'MISS', same pool object false
UnserializeWritesFetchedKey store: hook true, fetched the stored value true, then 'stored by hook', sibling 'sibling', same pool object true
UnserializeWritesFetchedKey delete: hook true, fetched the stored value true, then 'MISS', sibling 'sibling', same pool object true
UnserializeWritesFetchedKey clear: hook true, fetched the stored value true, then 'MISS', sibling 'MISS', same pool object true
UnserializeWritesFetchedKey deletePool: hook true, fetched the stored value true, then 'MISS', sibling 'MISS', same pool object false

graph restore hook bailout:
Outer::__wakeup

Fatal error: Cannot redeclare class GraphRestoreHookBailout (previously declared in %s) in %s on line 1
