--TEST--
UserCache\Cache: internal prototypes do not create extra objects with destructors, including nested native state, and release rejected cyclic prototypes and cycles through SPL internal storage
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=16M
error_reporting=E_ALL & ~E_DEPRECATED
--FILE--
<?php
class PrototypeDestructorLog
{
    public static string $phase = 'source';
    public static array $events = [];
    public static int $clones = 0;
}

class PrototypeDestructorValue
{
    public int $value = 41;

    public function __destruct()
    {
        PrototypeDestructorLog::$events[] = PrototypeDestructorLog::$phase;
    }

    public function __clone()
    {
        PrototypeDestructorLog::$clones++;
    }
}

class InheritedPrototypeDestructor extends PrototypeDestructorValue {}

class PrototypeDestructorArrayObject extends ArrayObject
{
    public function __destruct()
    {
        PrototypeDestructorLog::$events[] = PrototypeDestructorLog::$phase;
    }
}

function exercise(string $name, callable $make, callable $check, bool $clear): void
{
    $cache = UserCache\Cache::getPool('prototype-dtor-' . $name);
    PrototypeDestructorLog::$phase = 'source';
    PrototypeDestructorLog::$events = [];
    PrototypeDestructorLog::$clones = 0;
    $source = $make();
    if (!$cache->store('value', $source)) {
        throw new RuntimeException('store failed');
    }
    unset($source);
    gc_collect_cycles();
    $sourceEvents = PrototypeDestructorLog::$events;

    PrototypeDestructorLog::$phase = 'first logical request';
    PrototypeDestructorLog::$events = [];
    $valuesOk = true;
    for ($i = 0; $i < 2; $i++) {
        $fetched = $cache->fetch('value');
        $valuesOk = $check($fetched) && $valuesOk;
        unset($fetched);
        gc_collect_cycles();
    }
    $fetchEvents = PrototypeDestructorLog::$events;

    PrototypeDestructorLog::$phase = 'next logical request';
    PrototypeDestructorLog::$events = [];
    $clear ? $cache->clear() : $cache->delete('value');
    gc_collect_cycles();

    echo $name, ': ';
    var_dump($sourceEvents === ['source']
        && $fetchEvents === ['first logical request', 'first logical request']
        && PrototypeDestructorLog::$events === []
        && PrototypeDestructorLog::$clones === 0
        && $valuesOk);
}

exercise('direct', fn() => new PrototypeDestructorValue,
    fn($v) => $v instanceof PrototypeDestructorValue && $v->value === 41, true);
exercise('inherited', fn() => new InheritedPrototypeDestructor,
    fn($v) => $v instanceof InheritedPrototypeDestructor && $v->value === 41, false);
exercise('visible-alias', function () {
    $child = new PrototypeDestructorValue;
    return (object) ['values' => [new stdClass, $child], 'alias' => $child];
}, fn($v) => $v->values[1] === $v->alias && $v->alias->value === 41, true);
exercise('cyclic-array', function () {
    $values = [];
    $values['self'] =& $values;
    $values['child'] = new PrototypeDestructorValue;
    return $values;
}, fn($v) => $v['self']['child'] === $v['child'] && $v['child']->value === 41, false);

/* The destructor-bearing child is in native state, not declared properties. */
exercise('array-object', fn() => new ArrayObject([new stdClass, new PrototypeDestructorValue]),
    fn($v) => $v instanceof ArrayObject && $v[1]->value === 41, false);
exercise('fixed-array', fn() => SplFixedArray::fromArray([new stdClass, new PrototypeDestructorValue]),
    fn($v) => $v instanceof SplFixedArray && $v[1]->value === 41, true);
exercise('stack', function () {
    $stack = new SplStack;
    $stack->push(new stdClass);
    $stack->push(new PrototypeDestructorValue);
    return $stack;
}, fn($v) => $v instanceof SplStack && $v->top()->value === 41, false);
exercise('object-storage', function () {
    $storage = new SplObjectStorage;
    $storage->attach(new PrototypeDestructorValue);
    return $storage;
}, function ($v) {
    $v->rewind();
    return $v instanceof SplObjectStorage && $v->current()->value === 41;
}, true);

/* The native container itself may inherit or declare a user destructor. */
exercise('native-subclass', fn() => new PrototypeDestructorArrayObject([41]),
    fn($v) => $v instanceof PrototypeDestructorArrayObject && $v[0] === 41, false);

function request_local_prototype_internal_cycle(): void
{
    $cache = UserCache\Cache::getPool('request-local-prototype-internal-cycle');

    $containers = [
        'SplDoublyLinkedList' => static function (stdClass $item): object {
            $list = new SplDoublyLinkedList;
            $list->push($item);

            return $list;
        },
        'ArrayObject' => static function (stdClass $item): object {
            return new ArrayObject(['item' => $item]);
        },
        'SplFixedArray' => static function (stdClass $item): object {
            $array = new SplFixedArray(1);
            $array[0] = $item;

            return $array;
        },
    ];

    foreach ($containers as $name => $wrap) {
        $item = new stdClass;
        $item->container = $wrap($item);
        var_dump($cache->store($name, $item));

        for ($round = 0; $round < 3; $round++) {
            $fetched = $cache->fetch($name);
            foreach ($fetched->container as $inner) {
                echo $name, ' #', $round, ': ', $inner === $fetched ? 'cycle kept' : 'cycle lost', "\n";
            }
        }
    }

    /* Release builds do not collect cycles at shutdown. */
    $item = $fetched = $inner = null;
    gc_collect_cycles();
}

echo "\nrequest-local prototype internal cycle:\n";
request_local_prototype_internal_cycle();

class PartialPrototypeDestructor
{
    public static string $phase = 'source';

    public function __destruct()
    {
        echo 'destructor: ', self::$phase, "\n";
    }
}

class NativePartialPrototypeDestructor
{
    public static int $count = 0;

    public function __destruct()
    {
        self::$count++;
    }
}

function releaseCycle(array &$value): void
{
    /* The decoded root is a copy of the array referenced by its self edge.
     * Break both edges so no ordinary user-owned cycle remains for GC. */
    unset($value['self']['self'], $value['self']);
}

function releaseNativeCycle(object $container): void
{
    $value = $container[0];
    unset($value['self']['self'], $value['self'], $container[0]);
}

function prototype_destructor_partial_cycle(): void
{
    echo "array root\n";
    $cache = UserCache\Cache::getPool('partial-prototype-cycle');
    $source = [];
    $source['self'] =& $source;
    $source['child'] = new PartialPrototypeDestructor;
    $cache->store('value', $source);
    releaseCycle($source);
    unset($source);
    gc_collect_cycles();
    gc_disable();

    foreach (['first fetch', 'prototype rejection', 'later fetch'] as $phase) {
        PartialPrototypeDestructor::$phase = $phase;
        $fetched = $cache->fetch('value');
        releaseCycle($fetched);
        unset($fetched);
        echo $phase, " finished\n";
    }

    PartialPrototypeDestructor::$phase = 'next logical request';
    $cache->clear();
    gc_collect_cycles();
    echo "next logical request finished\n";

    echo "native container state\n";
    foreach (['ArrayObject', 'SplFixedArray'] as $class) {
        $cache = UserCache\Cache::getPool('native-partial-cycle-' . $class);
        NativePartialPrototypeDestructor::$count = 0;
        $source = [];
        $source['self'] =& $source;
        $source['child'] = new NativePartialPrototypeDestructor;
        $container = $class === 'ArrayObject'
            ? new ArrayObject([$source])
            : SplFixedArray::fromArray([$source]);
        $cache->store('value', $container);
        releaseNativeCycle($container);
        unset($source, $container);
        gc_collect_cycles();

        $valid = NativePartialPrototypeDestructor::$count === 1;
        for ($i = 0; $i < 3; $i++) {
            $fetched = $cache->fetch('value');
            $value = $fetched[0];
            $valid = $valid && $value['self']['child'] === $value['child'];
            unset($value);
            releaseNativeCycle($fetched);
            unset($fetched);
            $valid = $valid && NativePartialPrototypeDestructor::$count === $i + 2;
        }

        $cache->clear();
        gc_collect_cycles();
        echo $class, ': ';
        var_dump($valid && NativePartialPrototypeDestructor::$count === 4);
    }
}

echo "\nprototype destructor partial cycle:\n";
prototype_destructor_partial_cycle();
?>
--EXPECT--
direct: bool(true)
inherited: bool(true)
visible-alias: bool(true)
cyclic-array: bool(true)
array-object: bool(true)
fixed-array: bool(true)
stack: bool(true)
object-storage: bool(true)
native-subclass: bool(true)

request-local prototype internal cycle:
bool(true)
SplDoublyLinkedList #0: cycle kept
SplDoublyLinkedList #1: cycle kept
SplDoublyLinkedList #2: cycle kept
bool(true)
ArrayObject #0: cycle kept
ArrayObject #1: cycle kept
ArrayObject #2: cycle kept
bool(true)
SplFixedArray #0: cycle kept
SplFixedArray #1: cycle kept
SplFixedArray #2: cycle kept

prototype destructor partial cycle:
array root
destructor: source
destructor: first fetch
first fetch finished
destructor: prototype rejection
prototype rejection finished
destructor: later fetch
later fetch finished
next logical request finished
native container state
ArrayObject: bool(true)
SplFixedArray: bool(true)
