--TEST--
UserCache\Cache: request-local slots stay serialize-parity-safe across fetches, overwrites, storage transitions, restore failures, deprecations, enums, native state and native __wakeup() classes
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=16M
date.timezone=UTC
--FILE--
<?php
use UserCache\Cache;

function request_local_clone_array_object(): void
{
    $cache = UserCache\Cache::getPool('request-local-clone-array-object');
    $cache->clear();

    $shared = new stdClass;
    $shared->n = 1;
    $value = [
        'date' => new DateTimeImmutable('2020-01-01 00:00:00 UTC'),
        'storage' => new ArrayObject([$shared, 'nested' => [1, $shared]]),
        'shared' => $shared,
    ];
    var_dump($cache->store('graph', $value));
    unset($value, $shared);

    /* The second fetch builds the prototype and later fetches copy it; the ArrayObject copies its storage array. */
    for ($i = 0; $i < 4; $i++) {
        $fetched = $cache->fetch('graph');
        $storage = $fetched['storage'];
        var_dump(count($storage), $storage[0] === $fetched['shared'], $storage['nested'][1] === $fetched['shared']);
        unset($fetched, $storage);
    }

    $before = memory_get_usage();
    for ($i = 0; $i < 100; $i++) {
        $cache->fetch('graph');
    }
    gc_collect_cycles();
    var_dump(getenv('USE_ZEND_ALLOC') === '0' || memory_get_usage() <= $before);
}

echo "clone ArrayObject:\n";
request_local_clone_array_object();

class Plain
{
    public $a = 1;
}

function request_local_prototype_dynamic_property_deprecation(): void
{
    $notices = [];
    set_error_handler(function (int $errno, string $message) use (&$notices): bool {
        $notices[] = $message;

        return true;
    });

    $object = new Plain;
    $object->dynamic = 'x';
    $cache = UserCache\Cache::getPool('prototype-dynamic-property');
    $value = [$object, new DateTimeImmutable('2020-01-01')];
    var_dump($cache->store('k', $value));
    $notices = [];

    for ($i = 0; $i < 4; $i++) {
        $notices = [];
        $fetched = $cache->fetch('k');
        echo "fetch $i: ", implode(' | ', $notices), ' ', $fetched[0]->dynamic, "\n";
    }
    $notices = [];
    unserialize(serialize($value));
    echo 'unserialize: ', implode(' | ', $notices), "\n";

    restore_error_handler();
}

echo "\ndynamic property deprecation:\n";
request_local_prototype_dynamic_property_deprecation();

enum Suit: string
{
    case Hearts = 'h';
}

function request_local_prototype_enum(): void
{
    $cache = UserCache\Cache::getPool('prototype-enum');
    $value = ['suit' => Suit::Hearts];
    for ($i = 0; $i < 200; $i++) {
        $value["o$i"] = new stdClass;
    }
    $cache->store('k', $value);
    unset($value);

    /* The first read marks a key, the second keeps a prototype of it. */
    $cache->fetch('k');
    $before = memory_get_usage();
    $cache->fetch('k');
    var_dump(getenv('USE_ZEND_ALLOC') === '0' || memory_get_usage() - $before > 16 * 1024);

    for ($i = 0; $i < 3; $i++) {
        $v = $cache->fetch('k');
        var_dump($v['suit'] === Suit::Hearts, $v['o0'] instanceof stdClass);
    }
}

echo "\nenum:\n";
request_local_prototype_enum();

class WakeupDate extends DateTimeImmutable
{
    public static int $woken = 0;

    public function __wakeup(): void
    {
        self::$woken++;
    }
}

function request_local_prototype_native_wakeup(): void
{
    $factories = [
        'DateTimeImmutable' => fn (int $i) => new DateTimeImmutable('@' . (1700000000 + $i)),
        'DateTime' => fn (int $i) => new DateTime('@' . (1700000000 + $i)),
        'DateTimeZone' => fn (int $i) => new DateTimeZone('Asia/Tokyo'),
        'DateInterval' => fn (int $i) => new DateInterval('PT' . ($i + 1) . 'S'),
        'DatePeriod' => fn (int $i) => new DatePeriod(new DateTimeImmutable('@' . $i), new DateInterval('P1D'), 3),
        'WakeupDate' => fn (int $i) => new WakeupDate('@' . $i),
    ];

    $cache = Cache::getPool('prototype-native-wakeup');
    foreach ($factories as $class => $factory) {
        $value = [];
        for ($i = 0; $i < 200; $i++) {
            $value[] = ['object' => $factory($i), 'i' => $i];
        }
        $cache->store($class, $value);

        /* The first read marks a key, the second keeps a prototype of it. */
        $cache->fetch($class);
        $before = memory_get_usage();
        $cache->fetch($class);
        $retained = memory_get_usage() - $before;

        $a = $cache->fetch($class);
        $b = $cache->fetch($class);
        echo $class, ': ';
        var_dump(getenv('USE_ZEND_ALLOC') === '0' || $retained > 32 * 1024, serialize($a) === serialize($value), $a[0]['object'] !== $b[0]['object']);
    }
    var_dump(WakeupDate::$woken);
}

echo "\nnative __wakeup():\n";
request_local_prototype_native_wakeup();

echo "\nserialize parity:\n";

/* Restore hooks run on every fetch and structural cloning never runs __clone(). */
class StrictParityMagicDate extends DateTime
{
    public static int $unserializeCalls = 0;
    public static int $cloneCalls = 0;

    public function __serialize(): array
    {
        return parent::__serialize();
    }

    public function __unserialize(array $data): void
    {
        self::$unserializeCalls++;
        parent::__unserialize($data);
    }

    public function __clone(): void
    {
        self::$cloneCalls++;
    }
}

class StrictParityPlainClone
{
    public static int $cloneCalls = 0;

    public function __construct(public int $value = 0, public array $tags = [])
    {
    }

    public function __clone(): void
    {
        self::$cloneCalls++;
    }
}

class StrictParitySleeper
{
    public static int $wakeupCalls = 0;

    public int $value = 0;

    public function __construct(int $value = 0)
    {
        $this->value = $value;
    }

    public function __sleep(): array
    {
        return ['value'];
    }

    public function __wakeup(): void
    {
        self::$wakeupCalls++;
    }
}

class StrictParityWakeupOnly
{
    public static int $wakeupCalls = 0;

    public int $value = 0;

    public function __construct(int $value = 0)
    {
        $this->value = $value;
    }

    public function __wakeup(): void
    {
        self::$wakeupCalls++;
    }
}

$cache = UserCache\Cache::getPool('slot-strict-parity');
$cache->clear();
StrictParityMagicDate::$unserializeCalls = 0;
StrictParityMagicDate::$cloneCalls = 0;
StrictParityPlainClone::$cloneCalls = 0;
StrictParitySleeper::$wakeupCalls = 0;
StrictParityWakeupOnly::$wakeupCalls = 0;

var_dump($cache->store('magic', new StrictParityMagicDate('2026-07-08 01:02:03.456789', new DateTimeZone('UTC'))));
$magic = null;
for ($i = 0; $i < 5; $i++) {
    $magic = $cache->fetch('magic');
}
echo "magic __unserialize calls: ", StrictParityMagicDate::$unserializeCalls, "\n";
echo "magic __clone calls: ", StrictParityMagicDate::$cloneCalls, "\n";
echo "magic round-trips: ";
var_dump($magic instanceof StrictParityMagicDate &&
    $magic->format('Y-m-d H:i:s.u') === '2026-07-08 01:02:03.456789');

var_dump($cache->store('plain', new StrictParityPlainClone(42, ['a', 'b'])));
$plainOk = true;
for ($i = 0; $i < 5; $i++) {
    $plain = $cache->fetch('plain');
    $plainOk = $plainOk && $plain->value === 42 && $plain->tags === ['a', 'b'];
}
echo "plain __clone calls: ", StrictParityPlainClone::$cloneCalls, "\n";
echo "plain round-trips: ";
var_dump($plainOk);

var_dump($cache->store('sleeper', new StrictParitySleeper(7)));
$sleeperOk = true;
for ($i = 0; $i < 5; $i++) {
    $sleeper = $cache->fetch('sleeper');
    $sleeperOk = $sleeperOk && $sleeper->value === 7;
}
echo "sleeper __wakeup calls: ", StrictParitySleeper::$wakeupCalls, "\n";
echo "sleeper round-trips: ";
var_dump($sleeperOk);

var_dump($cache->store('wakeup-only', new StrictParityWakeupOnly(9)));
$wakeupOnlyOk = true;
for ($i = 0; $i < 5; $i++) {
    $wakeupOnly = $cache->fetch('wakeup-only');
    $wakeupOnlyOk = $wakeupOnlyOk && $wakeupOnly->value === 9;
}
echo "wakeup-only __wakeup calls: ", StrictParityWakeupOnly::$wakeupCalls, "\n";
echo "wakeup-only round-trips: ";
var_dump($wakeupOnlyOk);

/* Store hooks run exactly once while restore hooks run on every fetch. */
class SlotMagicPair
{
    public static int $serializeCalls = 0;
    public static int $unserializeCalls = 0;

    public function __construct(public int $value = 0)
    {
    }

    public function __serialize(): array
    {
        self::$serializeCalls++;
        return ['value' => $this->value];
    }

    public function __unserialize(array $data): void
    {
        self::$unserializeCalls++;
        $this->value = $data['value'];
    }
}

class SlotSleepWakeup
{
    public static int $sleepCalls = 0;
    public static int $wakeupCalls = 0;

    public function __construct(public int $value = 0)
    {
    }

    public function __sleep(): array
    {
        self::$sleepCalls++;
        return ['value'];
    }

    public function __wakeup(): void
    {
        self::$wakeupCalls++;
    }
}

$cache = UserCache\Cache::getPool('slot-magic');
$cache->clear();
SlotMagicPair::$serializeCalls = 0;
SlotMagicPair::$unserializeCalls = 0;
SlotSleepWakeup::$sleepCalls = 0;
SlotSleepWakeup::$wakeupCalls = 0;

var_dump($cache->store('pair', new SlotMagicPair(11)));
$first = $cache->fetch('pair');
$second = $cache->fetch('pair');
$third = $cache->fetch('pair');
$fourth = $cache->fetch('pair');
echo "pair serialize calls: ", SlotMagicPair::$serializeCalls, "\n";
echo "pair unserialize calls: ", SlotMagicPair::$unserializeCalls, "\n";
echo "pair fetches independent: ";
var_dump($third !== $fourth && $third->value === 11 && $fourth->value === 11);
$third->value = 99;
echo "pair mutation isolated: ";
var_dump($cache->fetch('pair')->value === 11);

var_dump($cache->store('sleeper', new SlotSleepWakeup(33)));
$cache->fetch('sleeper');
$cache->fetch('sleeper');
$clone1 = $cache->fetch('sleeper');
$clone2 = $cache->fetch('sleeper');
echo "sleep calls: ", SlotSleepWakeup::$sleepCalls, "\n";
echo "wakeup calls: ", SlotSleepWakeup::$wakeupCalls, "\n";
echo "sleeper fetches independent: ";
var_dump($clone1 !== $clone2 && $clone1->value === 33 && $clone2->value === 33);

/* Same-request store() and delete() invalidate the promoted slot. */
class SlotInvalidateValue
{
    public static int $unserializeCalls = 0;

    public function __construct(public int $value = 0)
    {
    }

    public function __serialize(): array
    {
        return ['value' => $this->value];
    }

    public function __unserialize(array $data): void
    {
        self::$unserializeCalls++;
        $this->value = $data['value'];
    }
}

$cache = UserCache\Cache::getPool('slot-invalidate');
$cache->clear();
SlotInvalidateValue::$unserializeCalls = 0;

var_dump($cache->store('key', new SlotInvalidateValue(1)));
$cache->fetch('key');
$cache->fetch('key');
$promoted = $cache->fetch('key');
echo "calls after promotion: ", SlotInvalidateValue::$unserializeCalls, "\n";
echo "promoted value: ", $promoted->value, "\n";

var_dump($cache->store('key', new SlotInvalidateValue(2)));
$fresh = $cache->fetch('key');
echo "value after overwrite: ", $fresh->value, "\n";
echo "calls after overwrite fetch: ", SlotInvalidateValue::$unserializeCalls, "\n";

var_dump($cache->delete('key'));
var_dump($cache->fetch('key'));
var_dump($cache->store('key', new SlotInvalidateValue(3)));
echo "value after delete and re-store: ", $cache->fetch('key')->value, "\n";

/* One key alternates between string and graph values, keeping aliases. */
class SlotStorageNode
{
    public array $tags = ['original'];
}

$cache = UserCache\Cache::getPool('slot-storage');
$cache->clear();
$string = str_repeat('s', 300);
var_dump($cache->store('key', $string));
$copy = $cache->fetch('key');
$copy[0] = 'x';
var_dump($cache->fetch('key') === $string);

$node = new SlotStorageNode();
$shared = ['value' => 1];
$graph = ['first' => $node, 'second' => $node, 'left' => &$shared, 'right' => &$shared];
var_dump($cache->store('key', $graph));
$held = [];
for ($i = 0; $i < 3; $i++) {
    $held[] = $cache->fetch('key');
}
var_dump($held[0]['first'] === $held[0]['second']);
var_dump($held[1]['first'] === $held[1]['second']);
var_dump($held[2]['first'] === $held[2]['second']);
$held[0]['first']->tags[] = 'changed';
$held[0]['left']['value'] = 9;
var_dump($held[0]['right']['value'] === 9);
var_dump($held[1]['first']->tags === ['original'], $held[2]['left']['value'] === 1);

var_dump($cache->store('key', $string));
var_dump($cache->fetch('key') === $string);

var_dump($cache->store('key', $graph));
for ($i = 0; $i < 3; $i++) {
    $fetched = $cache->fetch('key');
}
var_dump($fetched['first']->tags === ['original']);
var_dump($fetched['first'] === $fetched['second']);
var_dump($cache->delete('key'), $cache->fetch('key', 'missing'));

/* Release builds do not collect cycles at shutdown. */
unset($fetched, $held, $graph, $node, $shared);
gc_collect_cycles();

/* Throwing restore hooks propagate from fetch(), remember() and fetchMultiple() and keep the entry. */
class RestoreWakeupThrows
{
    public static bool $failNext = false;
    public int $value = 0;

    public function __wakeup(): void
    {
        if (self::$failNext) {
            self::$failNext = false;
            throw new RuntimeException('wakeup failure');
        }
    }
}

$cache = UserCache\Cache::getPool('fetch-restore-exception');
$cache->clear();
RestoreWakeupThrows::$failNext = false;

var_dump($cache->store('k', new RestoreWakeupThrows()));

RestoreWakeupThrows::$failNext = true;
try {
    $cache->fetch('k', 'DEFAULT');
    echo "no exception\n";
} catch (RuntimeException $e) {
    echo "fetch caught: ", $e->getMessage(), "\n";
}
var_dump($cache->has('k'));

RestoreWakeupThrows::$failNext = true;
try {
    $cache->remember('k', fn (): string => 'computed');
    echo "no exception\n";
} catch (RuntimeException $e) {
    echo "remember caught: ", $e->getMessage(), "\n";
}
var_dump($cache->has('k'));

RestoreWakeupThrows::$failNext = true;
try {
    $cache->fetchMultiple(['k'], 'DEFAULT');
    echo "no exception\n";
} catch (RuntimeException $e) {
    echo "fetchMultiple caught: ", $e->getMessage(), "\n";
}
var_dump($cache->has('k'));

$restored = $cache->fetch('k', 'DEFAULT');
var_dump($restored instanceof RestoreWakeupThrows);
?>
--EXPECT--
clone ArrayObject:
bool(true)
int(2)
bool(true)
bool(true)
int(2)
bool(true)
bool(true)
int(2)
bool(true)
bool(true)
int(2)
bool(true)
bool(true)
bool(true)

dynamic property deprecation:
bool(true)
fetch 0: Creation of dynamic property Plain::$dynamic is deprecated x
fetch 1: Creation of dynamic property Plain::$dynamic is deprecated x
fetch 2: Creation of dynamic property Plain::$dynamic is deprecated x
fetch 3: Creation of dynamic property Plain::$dynamic is deprecated x
unserialize: Creation of dynamic property Plain::$dynamic is deprecated

enum:
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

native __wakeup():
DateTimeImmutable: bool(true)
bool(true)
bool(true)
DateTime: bool(true)
bool(true)
bool(true)
DateTimeZone: bool(true)
bool(true)
bool(true)
DateInterval: bool(true)
bool(true)
bool(true)
DatePeriod: bool(true)
bool(true)
bool(true)
WakeupDate: bool(true)
bool(true)
bool(true)
int(0)

serialize parity:
bool(true)
magic __unserialize calls: 5
magic __clone calls: 0
magic round-trips: bool(true)
bool(true)
plain __clone calls: 0
plain round-trips: bool(true)
bool(true)
sleeper __wakeup calls: 5
sleeper round-trips: bool(true)
bool(true)
wakeup-only __wakeup calls: 5
wakeup-only round-trips: bool(true)
bool(true)
pair serialize calls: 1
pair unserialize calls: 4
pair fetches independent: bool(true)
pair mutation isolated: bool(true)
bool(true)
sleep calls: 1
wakeup calls: 4
sleeper fetches independent: bool(true)
bool(true)
calls after promotion: 3
promoted value: 1
bool(true)
value after overwrite: 2
calls after overwrite fetch: 4
bool(true)
NULL
bool(true)
value after delete and re-store: 3
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
string(7) "missing"
bool(true)
fetch caught: wakeup failure
bool(true)
remember caught: wakeup failure
bool(true)
fetchMultiple caught: wakeup failure
bool(true)
bool(true)
