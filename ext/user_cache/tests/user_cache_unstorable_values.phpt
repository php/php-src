--TEST--
UserCache\Cache: opaque, lazy, magic-exposed, hook-inserted and uninitialized values are refused without a partial store
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=16M
date.timezone=UTC
error_reporting=E_ALL & ~E_DEPRECATED
--FILE--
<?php
/* Resources, closures and opaque internals are refused at the root, nested, in properties and in SPL containers */
$cache = UserCache\Cache::getPool('unstorable-opaque');

class UserCacheUnsupportedBox
{
    public function __construct(public mixed $value)
    {
    }
}

function attempt(UserCache\Cache $cache, string $label, mixed $value): void
{
    static $i = 0;

    $key = 'opaque-' . $i++;
    try {
        $outcome = $cache->store($key, $value) ? 'stored' : 'REFUSED';
    } catch (TypeError $e) {
        $outcome = 'TypeError' . ($cache->has($key) ? ' (partial store)' : '');
    }
    echo $label, ': ', $outcome, "\n";
}

$resource = fopen(__FILE__, 'r');
$closure = static fn () => true;

attempt($cache, 'resource', $resource);
attempt($cache, 'closure', $closure);
attempt($cache, 'nested resource', ['value' => $resource]);
attempt($cache, 'nested closure', ['value' => $closure]);
attempt($cache, 'object resource', new UserCacheUnsupportedBox($resource));
attempt($cache, 'object closure', new UserCacheUnsupportedBox($closure));
attempt($cache, 'fixed resource', SplFixedArray::fromArray([$resource], false));
attempt($cache, 'fixed closure', SplFixedArray::fromArray([$closure], false));
attempt($cache, 'array resource', new ArrayObject(['value' => $resource]));
attempt($cache, 'array closure', new ArrayObject(['value' => $closure]));

$fiber = new Fiber(function () { Fiber::suspend(); });
$fiber->start();
attempt($cache, 'Fiber', $fiber);
attempt($cache, 'Generator', (function () { yield 1; })());
attempt($cache, 'WeakMap', (function () { $m = new WeakMap(); $m[new stdClass()] = 1; return $m; })());

fclose($resource);

/* Lazy objects, opaque internal objects and WeakReference report exact messages */
$cache = UserCache\Cache::getPool('unstorable-lazy');

class LazyTarget
{
    public int $x = 1;
}

$reflector = new ReflectionClass(LazyTarget::class);

$values = [
    'ghost' => $reflector->newLazyGhost(function ($object) { $object->x = 2; }),
    'proxy' => $reflector->newLazyProxy(fn() => new LazyTarget()),
    'nested-ghost' => ['inner' => $reflector->newLazyGhost(function ($object) { $object->x = 2; })],
    'not-serializable' => UserCache\Cache::getStatus(),
    'weakref' => WeakReference::create(new LazyTarget()),
    'nested-weakref' => ['inner' => WeakReference::create(new LazyTarget())],
];

foreach ($values as $key => $value) {
    try {
        $cache->store($key, $value);
    } catch (TypeError $e) {
        echo $e->getMessage(), "\n";
    }
    var_dump($cache->has($key));
}

/* Unstorable values exposed through __serialize() and __sleep() */
$cache = UserCache\Cache::getPool('unstorable-magic');

class UserCacheSerializeResource
{
    public $value;

    public function __serialize(): array
    {
        return ['value' => $this->value];
    }

    public function __unserialize(array $data): void
    {
        $this->value = $data['value'] ?? null;
    }
}

class UserCacheSleepResource
{
    public $value;

    public function __sleep(): array
    {
        return ['value'];
    }

    public function __wakeup(): void
    {
    }
}

function show(string $label, callable $callback): void
{
    try {
        var_dump($callback());
    } catch (TypeError $e) {
        echo $label, ": TypeError\n";
    }
}

$serializeResource = new UserCacheSerializeResource();
$serializeResource->value = fopen(__FILE__, 'r');
show('serialize-resource', fn() => $cache->store('serialize-resource', $serializeResource));
fclose($serializeResource->value);

$serializeClosure = new UserCacheSerializeResource();
$serializeClosure->value = static fn() => null;
show('serialize-closure', fn() => $cache->store('serialize-closure', $serializeClosure));

$sleepResource = new UserCacheSleepResource();
$sleepResource->value = fopen(__FILE__, 'r');
show('sleep-resource', fn() => $cache->store('sleep-resource', $sleepResource));
fclose($sleepResource->value);

/* Uninitialized date objects throw DateObjectError like serialize() */
$cache = UserCache\Cache::getPool('unstorable-date');

foreach ([DateTime::class, DateTimeZone::class, DateInterval::class, DatePeriod::class] as $class) {
    $object = (new ReflectionClass($class))->newInstanceWithoutConstructor();

    try {
        $result = $cache->store('date', $object);

        echo $class, ': stored, returned ', var_export($result, true), "\n";
    } catch (Error $e) {
        echo $class, ': ', get_class($e), ', message ',
            str_contains($e->getMessage(), 'has not been correctly initialized') ? 'OK' : $e->getMessage(),
            "\n";
    }
}

var_dump($cache->has('date'));
var_dump($cache->store('scalar', 7));
var_dump($cache->fetch('scalar'));

class Holder
{
    public array $list = [];
    public $hook;
}

class Inserts
{
    public static Holder $holder;
    public static Closure $factory;

    public function __serialize(): array
    {
        self::$holder->list[] = (self::$factory)();

        return [];
    }

    public function __unserialize(array $data): void
    {
    }
}

function store_hook_inserts_unstorable(): void
{
    $cache = UserCache\Cache::getPool('store-hook-inserts-unstorable');
    $factories = [
        'closure' => fn () => function () {},
        'resource' => fn () => fopen('php://memory', 'r'),
        'generator' => fn () => (function () { yield 1; })(),
        'anonymous class' => fn () => new class {},
        'WeakReference' => fn () => WeakReference::create(new stdClass),
    ];
    foreach ($factories as $name => $factory) {
        $holder = new Holder;
        $holder->hook = new Inserts;
        Inserts::$holder = $holder;
        Inserts::$factory = $factory;
        foreach (['inserted by a hook' => $holder, 'stored directly' => [$factory()]] as $how => $value) {
            try {
                var_dump($cache->store($name, $value));
            } catch (TypeError $e) {
                echo "$name $how: ", $e->getMessage(), "\n";
            }
        }
        var_dump($cache->has($name));
    }

    /* Release builds do not collect cycles at shutdown. */
    $holder = $value = $e = null;
    gc_collect_cycles();
}

echo "\nstore hook inserts unstorable:\n";
store_hook_inserts_unstorable();

echo "\nroot resource and Closure through every storing method:\n";
$cache = UserCache\Cache::getPool('unstorable-root-messages');
$resource = fopen(__FILE__, 'r');
foreach (['resource' => $resource, 'Closure' => static fn () => true] as $label => $value) {
    $calls = [
        'store' => fn () => $cache->store('root', $value),
        'add' => fn () => $cache->add('root', $value),
        'storeMultiple' => fn () => $cache->storeMultiple(['root' => $value]),
        'remember' => fn () => $cache->remember('root', fn () => $value),
    ];
    foreach ($calls as $method => $call) {
        try {
            var_dump($call());
        } catch (TypeError $e) {
            echo "$label $method: ", $e->getMessage(), "\n";
        }
    }
}
var_dump($cache->has('root'));
fclose($resource);
?>
--EXPECT--
resource: TypeError
closure: TypeError
nested resource: TypeError
nested closure: TypeError
object resource: TypeError
object closure: TypeError
fixed resource: TypeError
fixed closure: TypeError
array resource: TypeError
array closure: TypeError
Fiber: TypeError
Generator: TypeError
WeakMap: TypeError
Uninitialized lazy objects cannot be stored in the user cache
bool(false)
Uninitialized lazy objects cannot be stored in the user cache
bool(false)
Uninitialized lazy objects cannot be stored in the user cache
bool(false)
UserCache\CacheStatus objects cannot be stored in the user cache
bool(false)
WeakReference objects cannot be stored in the user cache
bool(false)
WeakReference objects cannot be stored in the user cache
bool(false)
serialize-resource: TypeError
serialize-closure: TypeError
sleep-resource: TypeError
DateTime: DateObjectError, message OK
DateTimeZone: DateObjectError, message OK
DateInterval: DateObjectError, message OK
DatePeriod: DateObjectError, message OK
bool(false)
bool(true)
int(7)

store hook inserts unstorable:
closure inserted by a hook: Closure objects cannot be stored in the user cache
closure stored directly: Closure objects cannot be stored in the user cache
bool(false)
resource inserted by a hook: Resources cannot be stored in the user cache
resource stored directly: Resources cannot be stored in the user cache
bool(false)
generator inserted by a hook: Generator objects cannot be stored in the user cache
generator stored directly: Generator objects cannot be stored in the user cache
bool(false)
anonymous class inserted by a hook: class@anonymous objects cannot be stored in the user cache
anonymous class stored directly: class@anonymous objects cannot be stored in the user cache
bool(false)
WeakReference inserted by a hook: WeakReference objects cannot be stored in the user cache
WeakReference stored directly: WeakReference objects cannot be stored in the user cache
bool(false)

root resource and Closure through every storing method:
resource store: Resources cannot be stored in the user cache
resource add: Resources cannot be stored in the user cache
resource storeMultiple: Resources cannot be stored in the user cache
resource remember: Resources cannot be stored in the user cache
Closure store: Closure objects cannot be stored in the user cache
Closure add: Closure objects cannot be stored in the user cache
Closure storeMultiple: Closure objects cannot be stored in the user cache
Closure remember: Closure objects cannot be stored in the user cache
bool(false)
