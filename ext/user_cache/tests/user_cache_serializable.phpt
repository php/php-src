--TEST--
UserCache\Cache: legacy Serializable keeps its handler contract, native precedence over magic methods, unserialize() failure semantics and its own native serialize() context across nesting, Fibers and fatal errors
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=16M
date.timezone=UTC
error_reporting=E_ALL & ~E_DEPRECATED
--FILE--
<?php
use UserCache\Cache;

function ok(string $label, bool $cond): void
{
    echo $label, ': ', $cond ? 'OK' : 'FAIL', "\n";
}

/* Serializable::serialize()/unserialize() drive store and every fetch */
$cache = UserCache\Cache::getPool('serializable-legacy');

class LegacyScalar implements Serializable
{
    public static int $unserializeCalls = 0;

    public function __construct(public int $value = 0)
    {
    }

    public function serialize(): string
    {
        return (string) $this->value;
    }

    public function unserialize(string $data): void
    {
        self::$unserializeCalls++;
        $this->value = (int) $data;
    }
}

LegacyScalar::$unserializeCalls = 0;
$scalar = new LegacyScalar(42);
$cache->store('scalar', $scalar);
$fetched = $cache->fetch('scalar');
ok('scalar instanceof', $fetched instanceof LegacyScalar);
ok('scalar value', $fetched->value === 42);
ok('scalar unserialize ran once per fetch', LegacyScalar::$unserializeCalls === 1);
ok('scalar parity', serialize($fetched) === serialize($scalar));

class Partial implements Serializable
{
    public int $kept = 0;
    public int $notRestored = 0;

    public function serialize(): string
    {
        return (string) $this->kept;
    }

    public function unserialize(string $data): void
    {
        $this->kept = (int) $data;
    }
}

$partial = new Partial();
$partial->kept = 5;
$partial->notRestored = 99;
$cache->store('partial', $partial);
$fetchedPartial = $cache->fetch('partial');
$nativePartial = unserialize(serialize($partial));
ok('partial matches native', $fetchedPartial->kept === $nativePartial->kept
    && $fetchedPartial->notRestored === $nativePartial->notRestored);

class LegacyBag implements Serializable
{
    public array $items = [];

    public function serialize(): string
    {
        return serialize($this->items);
    }

    public function unserialize(string $data): void
    {
        $this->items = unserialize($data);
    }
}

$bag = new LegacyBag();
$bag->items = ['a' => 1, 'b' => [2, 3], 'when' => new DateTimeImmutable('2026-01-01', new DateTimeZone('UTC'))];
$cache->store('bag', $bag);
$fetched = $cache->fetch('bag');
ok('bag instanceof', $fetched instanceof LegacyBag);
ok('bag array', $fetched->items['a'] === 1 && $fetched->items['b'] === [2, 3]);
ok('bag nested object', $fetched->items['when'] instanceof DateTimeImmutable
    && $fetched->items['when']->format('Y-m-d') === '2026-01-01');
ok('bag parity', serialize($fetched) === serialize($bag));

$graph = ['first' => new LegacyScalar(7), 'list' => [new LegacyScalar(8), new LegacyScalar(9)]];
$cache->store('graph', $graph);
$fetched = $cache->fetch('graph');
ok('graph values', $fetched['first']->value === 7
    && $fetched['list'][0]->value === 8
    && $fetched['list'][1]->value === 9);
ok('graph parity', serialize($fetched) === serialize($graph));

/* Serializable outranks __sleep/__wakeup/__unserialize; __serialize/__unserialize outrank Serializable */
$cache = UserCache\Cache::getPool('serializable-precedence');

class PrecedenceSerVsSleep implements Serializable
{
    public int $kept = 0;
    public string $via = 'none';

    public function serialize(): string
    {
        return (string) $this->kept;
    }

    public function unserialize(string $data): void
    {
        $this->kept = (int) $data;
        $this->via = 'iface';
    }

    public function __sleep(): array
    {
        return ['kept'];
    }

    public function __wakeup(): void
    {
        $this->via = 'sleep';
    }
}

class PrecedenceSerVsWakeup implements Serializable
{
    public int $kept = 0;
    public string $via = 'none';

    public function serialize(): string
    {
        return (string) $this->kept;
    }

    public function unserialize(string $data): void
    {
        $this->kept = (int) $data;
        $this->via = 'iface';
    }

    public function __wakeup(): void
    {
        $this->via = 'wakeup';
    }
}

class PrecedenceSerVsMagicUnserialize implements Serializable
{
    public int $kept = 0;
    public string $via = 'none';

    public function serialize(): string
    {
        return (string) $this->kept;
    }

    public function unserialize(string $data): void
    {
        $this->kept = (int) $data;
        $this->via = 'iface';
    }

    public function __unserialize(array $data): void
    {
        $this->via = 'magic';
    }
}

function check_precedence(UserCache\Cache $cache, string $label, object $value): void
{
    $native = unserialize(serialize($value));
    $cache->store($label, $value);
    $fetched = $cache->fetch($label);

    ok($label . ' instanceof', $fetched instanceof $value);
    ok($label . ' interface won (native parity)',
        $fetched->via === $native->via && $fetched->via === 'iface');
    ok($label . ' value', $fetched->kept === $native->kept);
}

$a = new PrecedenceSerVsSleep();
$a->kept = 5;
check_precedence($cache, 'PrecedenceSerVsSleep', $a);

$b = new PrecedenceSerVsWakeup();
$b->kept = 6;
check_precedence($cache, 'PrecedenceSerVsWakeup', $b);

$c = new PrecedenceSerVsMagicUnserialize();
$c->kept = 7;
check_precedence($cache, 'PrecedenceSerVsMagicUnserialize', $c);

class PrecedenceMagicOverIface implements Serializable
{
    public int $kept = 0;
    public string $via = 'none';

    public function serialize(): string
    {
        return 'IFACE';
    }

    public function unserialize(string $data): void
    {
        $this->via = 'iface';
    }

    public function __serialize(): array
    {
        return ['kept' => $this->kept];
    }

    public function __unserialize(array $data): void
    {
        $this->kept = $data['kept'];
        $this->via = 'magic';
    }
}

$d = new PrecedenceMagicOverIface();
$d->kept = 8;
$native = unserialize(serialize($d));
$cache->store('magic-over-iface', $d);
$fetched = $cache->fetch('magic-over-iface');
ok('magic outranks interface', $fetched->via === $native->via && $fetched->via === 'magic');
ok('magic value', $fetched->kept === $native->kept && $fetched->kept === 8);

class SuspendsInCacheSerialize implements Serializable
{
    public function serialize()
    {
        Fiber::suspend('cache');

        return 'a';
    }

    public function unserialize($data) {}
}

class SuspendsInNativeSerialize implements Serializable
{
    public function serialize()
    {
        Fiber::suspend('native');

        return 'b';
    }

    public function unserialize($data) {}
}

function serdes_fiber_switch(): void
{
    $cache = UserCache\Cache::getPool('serdes-fiber-switch');
    $native = new Fiber(fn () => serialize([new SuspendsInNativeSerialize()]));
    $store = new Fiber(function () use ($cache) {
        try {
            return $cache->store('key', [new SuspendsInCacheSerialize()]);
        } catch (FiberError $e) {
            return $e::class . ': ' . $e->getMessage();
        }
    });

    var_dump($native->start(), $store->start());
    $native->resume();
    var_dump($native->getReturn(), $store->getReturn());

    $shared = new stdClass();
    echo serialize([$shared, $shared, 1, 'x']), "\n";
    var_dump(unserialize(serialize([$shared, $shared]))[1] instanceof stdClass, $cache->has('key'));
}

echo "\nserdes fiber switch:\n";
serdes_fiber_switch();

class Throws implements Serializable
{
    public function serialize()
    {
        throw new DomainException('boom');
    }

    public function unserialize($data)
    {
    }
}

function serializable_exception(): void
{
    $cache = UserCache\Cache::getPool('serializable-exception');
    $attempts = [
        'store' => fn () => $cache->store('k', new Throws),
        'nested' => fn () => $cache->store('k', ['inner' => new Throws]),
        'storeMultiple' => fn () => $cache->storeMultiple(['k' => new Throws]),
        'remember' => fn () => $cache->remember('r', fn () => new Throws),
        'native' => fn () => serialize(new Throws),
    ];
    foreach ($attempts as $name => $attempt) {
        try {
            $attempt();
        } catch (Throwable $e) {
            echo $name, ': ', get_class($e), ' ', $e->getMessage(), ' previous=', var_export($e->getPrevious(), true), "\n";
        }
    }
    var_dump($cache->has('k'), $cache->has('r'));
}

echo "\nserializable exception:\n";
serializable_exception();

class Thrower implements Serializable {
    public function serialize() { return 'x'; }
    public function unserialize($data) { throw new RuntimeException('cannot restore'); }
}
class Victim {
    public $inner;
    public function __construct() { $this->inner = new Thrower(); }
    public function __wakeup() { echo "wakeup ran\n"; }
    public function __destruct() { echo "destructor ran\n"; }
}

function restore_failure_skips_destructor(): void
{
    $cache = Cache::getPool('restore-failure-skips-destructor');
    var_dump($cache->store('victim', new Victim()));
    echo "fetching\n";
    try {
        var_dump($cache->fetch('victim', 'default'));
    } catch (Throwable $e) {
        echo get_class($e), ': ', $e->getMessage(), "\n";
    }
    echo "native\n";
    try { unserialize(serialize(new Victim())); } catch (Throwable $e) { echo get_class($e), ': ', $e->getMessage(), "\n"; }
    echo "done\n";
}

echo "\nrestore failure skips destructor:\n";
restore_failure_skips_destructor();

class Inner implements Serializable {
    public $v;
    public function serialize() { $o = new stdClass; return serialize([$o, $o]); }
    public function unserialize($data) { $this->v = unserialize($data); }
}
class FetchingOuter implements Serializable {
    public static $fetched;
    public function serialize() { return ''; }
    public function unserialize($data) { self::$fetched = UserCache\Cache::getPool('serializable-inside-native-serialize')->fetch('inner'); }
}
class StoringOuter implements Serializable {
    public function serialize() { UserCache\Cache::getPool('serializable-inside-native-serialize')->store('stored-inside', new Inner); return ''; }
    public function unserialize($data) {}
}

function serializable_inside_native_serialize(): void
{
    $cache = UserCache\Cache::getPool('serializable-inside-native-serialize');
    $cache->store('inner', new Inner);
    $outer = unserialize('a:2:{i:0;O:8:"stdClass":0:{}i:1;C:13:"FetchingOuter":0:{}}');
    $v = FetchingOuter::$fetched->v;
    var_dump($v[0] === $v[1], $v[0] !== $outer[0]);
    $o = new stdClass;
    $s = serialize([new StoringOuter, $o, $o]);
    var_dump($s);
    $u = unserialize($s);
    var_dump($u[1] === $u[2]);
    $w = $cache->fetch('stored-inside')->v;
    var_dump($w[0] === $w[1]);
}

echo "\nserializable inside native serialize:\n";
serializable_inside_native_serialize();

/* fetch() inside an outer serialize(): the restored object's unserialize() serializes its own value. */
class SerializesOnRestore implements Serializable
{
    public function serialize(): string
    {
        return 'x';
    }

    public function unserialize(string $data): void
    {
        serialize([new stdClass, new stdClass]);
    }
}

class FetchesWhileSerialized
{
    public $shared;

    public function __construct()
    {
        $this->shared = new stdClass;
    }

    public function __serialize(): array
    {
        Cache::getPool('native-context')->fetch('restore');

        return ['a' => $this->shared, 'filler' => new ArrayObject, 'b' => $this->shared];
    }

    public function __unserialize(array $data): void
    {
        $this->shared = $data;
    }
}

/* store() inside an outer unserialize(): the stored object's serialize() unserializes its own value. */
class UnserializesOnStore implements Serializable
{
    public static array $nested = [];

    public function serialize(): string
    {
        self::$nested[] = unserialize('a:2:{i:0;O:8:"stdClass":0:{}i:1;r:2;}');

        return 'x';
    }

    public function unserialize(string $data): void
    {
    }
}

class StoresWhileUnserialized
{
    public function __serialize(): array
    {
        return ['a' => new stdClass];
    }

    public function __unserialize(array $data): void
    {
        Cache::getPool('native-context')->store('stored' . count(UnserializesOnStore::$nested), new UnserializesOnStore);
    }
}

function serializable_native_context_isolation(): void
{
    $cache = Cache::getPool('native-context');
    $cache->clear();
    var_dump($cache->store('restore', new SerializesOnRestore));

    $serialized = serialize(new FetchesWhileSerialized);
    $restored = unserialize($serialized);
    var_dump($restored->shared['a'] === $restored->shared['b'], get_debug_type($restored->shared['b']));

    unserialize(serialize([new StoresWhileUnserialized, new StoresWhileUnserialized]));
    foreach (UnserializesOnStore::$nested as $nested) {
        var_dump(get_debug_type($nested[1]), $nested[0] === $nested[1]);
    }
}

echo "\nserializable native context isolation:\n";
serializable_native_context_isolation();

class FatalInCacheSerialize implements Serializable
{
    public function __construct(public stdClass $shared) {}

    public function serialize()
    {
        serialize([$this->shared]);
        eval('class SerdesSerializeBailoutRedeclared {} class SerdesSerializeBailoutRedeclared {}');

        return 'unreachable';
    }

    public function unserialize($data) {}
}

function serdes_serialize_bailout(): void
{
    $shared = new stdClass();

    register_shutdown_function(function () use ($shared) {
        echo "shutdown\n";

        $fiber = new Fiber(fn () => Fiber::suspend('suspended'));
        var_dump($fiber->start());

        echo serialize([$shared, $shared]), "\n";
    });

    UserCache\Cache::getPool('serdes-serialize-bailout')->store('key', new FatalInCacheSerialize($shared));
    echo "unreachable\n";
}

echo "\nserdes serialize bailout:\n";
serdes_serialize_bailout();
?>
--EXPECTF--
scalar instanceof: OK
scalar value: OK
scalar unserialize ran once per fetch: OK
scalar parity: OK
partial matches native: OK
bag instanceof: OK
bag array: OK
bag nested object: OK
bag parity: OK
graph values: OK
graph parity: OK
PrecedenceSerVsSleep instanceof: OK
PrecedenceSerVsSleep interface won (native parity): OK
PrecedenceSerVsSleep value: OK
PrecedenceSerVsWakeup instanceof: OK
PrecedenceSerVsWakeup interface won (native parity): OK
PrecedenceSerVsWakeup value: OK
PrecedenceSerVsMagicUnserialize instanceof: OK
PrecedenceSerVsMagicUnserialize interface won (native parity): OK
PrecedenceSerVsMagicUnserialize value: OK
magic outranks interface: OK
magic value: OK

serdes fiber switch:
string(6) "native"
NULL
string(48) "a:1:{i:0;C:25:"SuspendsInNativeSerialize":1:{b}}"
string(61) "FiberError: Cannot switch fibers in current execution context"
a:4:{i:0;O:8:"stdClass":0:{}i:1;r:2;i:2;i:1;i:3;s:1:"x";}
bool(true)
bool(false)

serializable exception:
store: DomainException boom previous=NULL
nested: DomainException boom previous=NULL
storeMultiple: DomainException boom previous=NULL
remember: DomainException boom previous=NULL
native: DomainException boom previous=NULL
bool(false)
bool(false)

restore failure skips destructor:
destructor ran
bool(true)
fetching
RuntimeException: cannot restore
native
destructor ran
RuntimeException: cannot restore
done

serializable inside native serialize:
bool(true)
bool(true)
string(65) "a:3:{i:0;C:12:"StoringOuter":0:{}i:1;O:8:"stdClass":0:{}i:2;r:3;}"
bool(true)
bool(true)

serializable native context isolation:
bool(true)
bool(true)
string(8) "stdClass"
string(8) "stdClass"
bool(true)
string(8) "stdClass"
bool(true)

serdes serialize bailout:

Fatal error: Cannot redeclare class SerdesSerializeBailoutRedeclared (previously declared in %s) in %s on line 1
shutdown
string(9) "suspended"
a:2:{i:0;O:8:"stdClass":0:{}i:1;r:2;}
