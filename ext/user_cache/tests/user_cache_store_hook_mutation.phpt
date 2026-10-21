--TEST--
UserCache\Cache: serialization hooks that grow, replace, drop or write the containers, references and objects being stored, or throw during sizing, leave store() and the live values intact
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=16M
error_reporting=E_ALL & ~E_DEPRECATED
zend.exception_ignore_args=1
--FILE--
<?php
class Holder
{
    public $list;
}

/* Grows the array that is being walked in place (refcount 1). */
class Grower
{
    public static ?Holder $owner = null;
    public static int $count = 500;

    public function __serialize(): array
    {
        for ($i = 0; $i < self::$count; $i++) {
            self::$owner->list[] = $i;
        }
        self::$count = 0;

        return ['x' => 1];
    }

    public function __unserialize(array $data): void
    {
    }
}

/* Replaces the array that is being walked, freeing it. */
class Replacer
{
    public static ?Holder $owner = null;

    public function __serialize(): array
    {
        self::$owner->list = ['replaced'];

        return [];
    }

    public function __unserialize(array $data): void
    {
    }
}

#[AllowDynamicProperties]
class GraphDate extends DateTime
{
}

/* Adds dynamic properties to the safe-direct object that is being walked. */
class DateMutator
{
    public ?GraphDate $date = null;
    public static int $count = 200;

    public function __serialize(): array
    {
        for ($i = 0; $i < self::$count; $i++) {
            $this->date->{"p$i"} = $i;
        }
        self::$count = 0;

        return ['x' => 1];
    }

    public function __unserialize(array $data): void
    {
    }
}

class Box
{
    public $item;
    public $pad = 'tail';
}

class Root
{
    public ?Box $box = null;
}

/* Drops the last reference to the plain object that is being walked. */
class Unlinker
{
    public static ?Root $root = null;

    public function __serialize(): array
    {
        self::$root->box = null;

        return [];
    }

    public function __unserialize(array $data): void
    {
    }
}

class Late
{
    public static ?stdClass $owner = null;

    public function __serialize(): array
    {
        for ($i = 0; $i < 300; $i++) {
            self::$owner->arr[] = $i;
        }

        return ['n' => 1];
    }

    public function __unserialize(array $data): void
    {
    }
}

/* Adds a new hooked object behind an array that CALC already walked, so the
 * copy pass runs a hook that grows the array it is writing. */
class Injector
{
    public function __serialize(): array
    {
        Late::$owner->arr[] = new Late();
        Late::$owner->arr[] = 't1';
        Late::$owner->arr[] = 't2';

        return ['b' => 1];
    }

    public function __unserialize(array $data): void
    {
    }
}

final class DropHolders
{
    public function __serialize(): array
    {
        $GLOBALS['holders'] = [];

        return ['dropped' => true];
    }

    public function __unserialize(array $data): void
    {
    }
}

#[AllowDynamicProperties]
class Order
{
    public $status = 'new';
    public $child;
}

#[AllowDynamicProperties]
class Stamp extends DateTimeImmutable
{
    public $label = 'draft';
    public $child;
}

class Line
{
    public $owner;

    public function __serialize(): array
    {
        $this->owner->touched = true;

        return [];
    }

    public function __unserialize(array $data): void
    {
    }
}

class SleepPlainHolder
{
    public $inner;
}

class SleepSnapshotHolder
{
    public $inner;

    public function __sleep(): array
    {
        return ['inner'];
    }

    public function __wakeup(): void
    {
    }
}

class SleepDropper
{
    public static $holder;

    public $a = 1;

    public function __sleep(): array
    {
        SleepDropper::$holder->inner = null;

        return ['a'];
    }
}

class SerializePlainHolder
{
    public $inner;
}

class SerializeSnapshotHolder
{
    public $inner;

    public function __serialize(): array
    {
        return ['inner' => $this->inner];
    }

    public function __unserialize(array $data): void
    {
        $this->inner = $data['inner'];
    }
}

class SerializeDropper
{
    public static $holder;

    public $a = 1;

    public function __serialize(): array
    {
        SerializeDropper::$holder->inner = null;

        return ['a' => $this->a];
    }

    public function __unserialize(array $data): void
    {
        $this->a = $data['a'];
    }
}

class SerializeTracked
{
    public static int $destroyed = 0;

    public function __serialize(): array
    {
        return ['value' => 7];
    }

    public function __destruct()
    {
        self::$destroyed++;
    }
}

class SerializeThrows
{
    public function __serialize(): array
    {
        throw new RuntimeException('prepare failed');
    }
}

class SerPlainHolder
{
    public $inner;
}

class SerDropper implements Serializable
{
    public static $holder;

    public $a = 1;

    public function serialize(): string
    {
        SerDropper::$holder->inner = null;

        return (string) $this->a;
    }

    public function unserialize(string $data): void
    {
        $this->a = (int) $data;
    }
}

class SerSnapshotHolder implements Serializable
{
    public $inner;

    public function serialize(): string
    {
        return serialize($this->inner);
    }

    public function unserialize(string $data): void
    {
        $this->inner = unserialize($data);
    }
}

class SerDropper2 implements Serializable
{
    public static $holder;

    public $a = 7;

    public function serialize(): string
    {
        SerDropper2::$holder->inner = null;

        return (string) $this->a;
    }

    public function unserialize(string $data): void
    {
        $this->a = (int) $data;
    }
}

function make(object $owner): object
{
    $line = new Line;
    $line->owner = $owner;
    $owner->child = $line;

    return $owner;
}

function ok(string $label, bool $cond): void
{
    echo $label, ': ', $cond ? 'OK' : 'FAIL', "\n";
}

function graph_encoder_hook_mutation(): void
{
    $cache = UserCache\Cache::getPool('graph-encoder-hook-mutation');

    $holder = new Holder();
    $holder->list = [new Grower(), 'a', 'b', 'c'];
    Grower::$owner = $holder;
    var_dump($cache->store('grow', $holder));
    var_dump(count($holder->list));
    var_dump($cache->store('grow', $holder));
    var_dump(count($cache->fetch('grow')->list));

    $holder = new Holder();
    $holder->list = [new Replacer(), str_repeat('y', 64)];
    Replacer::$owner = $holder;
    var_dump($cache->store('replace', $holder));
    var_dump($cache->fetch('replace')->list);

    $date = new GraphDate('2020-01-02 03:04:05', new DateTimeZone('UTC'));
    $mutator = new DateMutator();
    $mutator->date = $date;
    $date->a = $mutator;
    $date->b = 'tail';
    var_dump($cache->store('date', $date));
    var_dump(count(get_object_vars($date)));
    var_dump($cache->store('date', $date));
    $fetched = $cache->fetch('date');
    var_dump($fetched->format('Y-m-d H:i:s'), count(get_object_vars($fetched)), $fetched->b);

    $root = new Root();
    $root->box = new Box();
    $root->box->item = new Unlinker();
    Unlinker::$root = $root;
    var_dump($cache->store('unlink', $root));
    var_dump($cache->fetch('unlink'));

    $owner = new stdClass();
    $owner->arr = [];
    Late::$owner = $owner;
    var_dump($cache->store('late', [$owner, new Injector(), str_repeat('x', 20000)]));
    var_dump(count($owner->arr));

    /* Release builds do not collect cycles at shutdown. */
    $date = $mutator = $fetched = null;
    gc_collect_cycles();
}

function store_hook_drops_reference_holders(): void
{
    global $holders;

    $cache = UserCache\Cache::getPool('store-hook-drops-reference-holders');

    $holders = [];
    $references = [];
    for ($i = 0; $i < 100; $i++) {
        $holders[$i] = $i;
        $references[$i] = &$holders[$i];
    }
    $value = [$references, new DropHolders, $references];
    unset($references);

    var_dump($cache->store('value', $value));

    $fetched = $cache->fetch('value');
    $fetched[0][0] = 'changed';
    var_dump($fetched[2][0], $fetched[0][99]);
}

function store_hook_writes_container(): void
{
    $cache = UserCache\Cache::getPool('store-hook-writes-container');

    $order = make(new Order);
    /* The hook changes an object that was already visited, so the store itself may fail. */
    $cache->store('order', $order);
    $order->status = 'paid';
    var_dump(get_object_vars($order)['status'], ((array) $order)['status'], $order->touched);

    $stamp = make(new Stamp('2020-01-01'));
    $cache->store('stamp', $stamp);
    $stamp->label = 'final';
    var_dump(get_object_vars($stamp)['label']);

    /* Declared slots stay visible to the cycle collector. */
    $order = make(new Order);
    $cache->store('cycle', $order);
    $weak = WeakReference::create($order);
    $order = $stamp = null;
    gc_collect_cycles();
    var_dump($weak->get());
}

function store_hook_lifetimes(): void
{
    /* __sleep()/__serialize() releasing the only reference to $this while the store walks it */
    $cache = UserCache\Cache::getPool('hook-lifetimes-magic');

    $holder = new SleepPlainHolder();
    $holder->inner = new SleepDropper();
    SleepDropper::$holder = $holder;
    ok('sleep plain store', $cache->store('sleep-plain', $holder) === true);
    ok('sleep plain live property cleared', $holder->inner === null);
    $fetched = $cache->fetch('sleep-plain');
    ok('sleep plain fetched class', $fetched instanceof SleepPlainHolder);
    ok('sleep plain fetched property', $fetched->inner === null);

    $holder = new SleepSnapshotHolder();
    $holder->inner = new SleepDropper();
    SleepDropper::$holder = $holder;
    ok('sleep snapshot store', $cache->store('sleep-snapshot', $holder) === true);
    ok('sleep snapshot live property cleared', $holder->inner === null);
    $fetched = $cache->fetch('sleep-snapshot');
    ok('sleep snapshot fetched class', $fetched instanceof SleepSnapshotHolder);
    ok('sleep snapshot inner class', $fetched->inner instanceof SleepDropper);
    ok('sleep snapshot inner value', $fetched->inner->a === 1);

    $holder = new SerializePlainHolder();
    $holder->inner = new SerializeDropper();
    SerializeDropper::$holder = $holder;
    ok('serialize plain store', $cache->store('serialize-plain', $holder) === true);
    ok('serialize plain live property cleared', $holder->inner === null);
    $fetched = $cache->fetch('serialize-plain');
    ok('serialize plain fetched class', $fetched instanceof SerializePlainHolder);
    ok('serialize plain fetched property', $fetched->inner === null);

    $holder = new SerializeSnapshotHolder();
    $holder->inner = new SerializeDropper();
    SerializeDropper::$holder = $holder;
    ok('serialize snapshot store', $cache->store('serialize-snapshot', $holder) === true);
    ok('serialize snapshot live property cleared', $holder->inner === null);
    $fetched = $cache->fetch('serialize-snapshot');
    ok('serialize snapshot fetched class', $fetched instanceof SerializeSnapshotHolder);
    ok('serialize snapshot inner class', $fetched->inner instanceof SerializeDropper);
    ok('serialize snapshot inner value', $fetched->inner->a === 1);

    /* Sizing fails after the memo pins an earlier object, before a graph buffer exists. */
    SerializeTracked::$destroyed = 0;
    try {
        $cache->store('failed', [new SerializeTracked(), new SerializeThrows()]);
    } catch (RuntimeException $e) {
        echo $e->getMessage(), "\n";
    }
    gc_collect_cycles();
    ok('failed preparation releases memo', SerializeTracked::$destroyed === 1);
    ok('failed preparation leaves no entry', !$cache->has('failed'));
    ok('cache still usable', $cache->store('scalar', 7) && $cache->fetch('scalar') === 7);

    /* Release builds do not collect cycles at shutdown. */
    SleepDropper::$holder = null;
    SerializeDropper::$holder = null;
    unset($holder, $fetched);
    gc_collect_cycles();

    /* Serializable::serialize() releasing the only reference to $this while the encoder walks it */
    $cache = UserCache\Cache::getPool('hook-lifetimes-serializable');

    $holder = new SerPlainHolder();
    $holder->inner = new SerDropper();
    SerDropper::$holder = $holder;
    ok('serializable plain store', $cache->store('serdes-plain', $holder) === true);
    ok('serializable plain live property cleared', $holder->inner === null);
    $fetched = $cache->fetch('serdes-plain');
    ok('serializable plain fetched class', $fetched instanceof SerPlainHolder);
    ok('serializable plain fetched property', $fetched->inner === null);

    $holder = new SerSnapshotHolder();
    $holder->inner = new SerDropper2();
    SerDropper2::$holder = $holder;
    ok('serializable snapshot store', $cache->store('serdes-snapshot', $holder) === true);
    ok('serializable snapshot live property cleared', $holder->inner === null);
    $fetched = $cache->fetch('serdes-snapshot');
    ok('serializable snapshot fetched class', $fetched instanceof SerSnapshotHolder);
    ok('serializable snapshot inner class', $fetched->inner instanceof SerDropper2);
    ok('serializable snapshot inner value', $fetched->inner->a === 7);

    ok('cache still usable', $cache->store('scalar', 5) && $cache->fetch('scalar') === 5);

    /* Release builds do not collect cycles at shutdown. */
    SerDropper::$holder = null;
    SerDropper2::$holder = null;
    unset($holder, $fetched);
    gc_collect_cycles();
}

/* Appends to the ArrayObject being stored; ArrayObject writes its storage in place without separating it. */
class ArrayObjectGrower
{
    public ?ArrayObject $target = null;
    public string $keyPrefix = '';

    public function __serialize(): array
    {
        for ($i = 0; $i < 200; $i++) {
            if ($this->keyPrefix === '') {
                $this->target[] = str_repeat('x', 40) . $i;
            } else {
                $this->target[$this->keyPrefix . $i] = str_repeat('x', 40) . $i;
            }
        }

        return [];
    }

    public function __unserialize(array $data): void
    {
    }
}

function store_hook_writes_array_object(): void
{
    $cache = UserCache\Cache::getPool('hook-writes-array-object');
    $grower = new ArrayObjectGrower();
    $target = new ArrayObject();
    $grower->target = $target;
    $target['grower'] = $grower;
    $target['b'] = 'y';
    $target['c'] = 'z';

    ok('array object store', $cache->store('ao', $target) === true);
    ok('array object live storage grew', count($target) === 203);

    $fetched = $cache->fetch('ao');
    ok('array object fetched snapshot', $fetched instanceof ArrayObject && count($fetched) === 3);
    ok('array object fetched element', $fetched['grower'] instanceof ArrayObjectGrower && $fetched['c'] === 'z');

    $later = new ArrayObject(['b' => 'y', 'c' => 'z']);
    $laterGrower = new ArrayObjectGrower();
    $laterGrower->target = $later;
    ok('array object written after its state store', $cache->store('ao-later', [$later, $laterGrower]) === true);
    ok('array object written after its state snapshot', count($cache->fetch('ao-later')[0]) === 2);

    $self = new ArrayObject();
    $self->exchangeArray($self);
    $selfGrower = new ArrayObjectGrower();
    $selfGrower->target = $self;
    $selfGrower->keyPrefix = 'p';
    $self['grower'] = $selfGrower;
    $self['c'] = 'z';
    ok('self-backed array object store', $cache->store('ao-self', $self) === true);
    ok('self-backed array object fetched', $cache->fetch('ao-self') instanceof ArrayObject);

    /* Release builds do not collect cycles at shutdown. */
    $grower->target = $laterGrower->target = $selfGrower->target = null;
    unset($grower, $target, $fetched, $later, $laterGrower, $self, $selfGrower);
    gc_collect_cycles();
}

echo "graph encoder hook mutation:\n";
graph_encoder_hook_mutation();

echo "\nstore hook drops reference holders:\n";
store_hook_drops_reference_holders();

echo "\nstore hook writes container:\n";
store_hook_writes_container();

echo "\nstore hook lifetimes:\n";
store_hook_lifetimes();

echo "\nstore hook writes ArrayObject:\n";
store_hook_writes_array_object();
?>
--EXPECTF--
graph encoder hook mutation:
bool(true)
int(504)
bool(true)
int(504)
bool(true)
array(1) {
  [0]=>
  string(8) "replaced"
}
bool(true)
int(202)
bool(true)
string(19) "2020-01-02 03:04:05"
int(202)
string(4) "tail"
bool(true)
object(Root)#%d (1) {
  ["box"]=>
  NULL
}
bool(false)
int(303)

store hook drops reference holders:
bool(true)
int(0)
int(99)

store hook writes container:
string(4) "paid"
string(4) "paid"
bool(true)
string(5) "final"
NULL

store hook lifetimes:
sleep plain store: OK
sleep plain live property cleared: OK
sleep plain fetched class: OK
sleep plain fetched property: OK
sleep snapshot store: OK
sleep snapshot live property cleared: OK
sleep snapshot fetched class: OK
sleep snapshot inner class: OK
sleep snapshot inner value: OK
serialize plain store: OK
serialize plain live property cleared: OK
serialize plain fetched class: OK
serialize plain fetched property: OK
serialize snapshot store: OK
serialize snapshot live property cleared: OK
serialize snapshot fetched class: OK
serialize snapshot inner class: OK
serialize snapshot inner value: OK
prepare failed
failed preparation releases memo: OK
failed preparation leaves no entry: OK
cache still usable: OK
serializable plain store: OK
serializable plain live property cleared: OK
serializable plain fetched class: OK
serializable plain fetched property: OK
serializable snapshot store: OK
serializable snapshot live property cleared: OK
serializable snapshot fetched class: OK
serializable snapshot inner class: OK
serializable snapshot inner value: OK
cache still usable: OK

store hook writes ArrayObject:
array object store: OK
array object live storage grew: OK
array object fetched snapshot: OK
array object fetched element: OK
array object written after its state store: OK
array object written after its state snapshot: OK
self-backed array object store: OK
self-backed array object fetched: OK
