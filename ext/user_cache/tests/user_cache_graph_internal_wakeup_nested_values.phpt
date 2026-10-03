--TEST--
UserCache\Cache: internal __wakeup classes round-trip nested floats, references, magic and __sleep objects like unserialize(serialize())
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=16M
zend.exception_ignore_args=0
--FILE--
<?php
function ok(string $label, bool $cond): void
{
    echo $label, ': ', $cond ? 'OK' : 'FAIL', "\n";
}

function with_trace(Throwable $e, array $trace): Throwable
{
    $class = $e instanceof Exception ? Exception::class : Error::class;
    (new ReflectionProperty($class, 'trace'))->setValue($e, $trace);

    return $e;
}

function native_copy(mixed $value): mixed
{
    return unserialize(serialize($value));
}

class SerdesMagic
{
    public static array $log = [];

    public function __construct(public mixed $payload = null)
    {
    }

    public function __serialize(): array
    {
        return ['payload' => $this->payload, 'ratio' => 0.75];
    }

    public function __unserialize(array $data): void
    {
        self::$log[] = 'unserialize';
        $this->payload = $data;
    }
}

class SerdesSleeper
{
    public int $kept = 1;
    public float $weight = 2.5;
    public string $state = 'built';

    public function __sleep(): array
    {
        return ['kept', 'weight'];
    }

    public function __wakeup(): void
    {
        SerdesMagic::$log[] = 'wakeup';
        $this->state = 'woken';
    }
}

enum SerdesSuit: string
{
    case Hearts = 'H';
}

/* Exception and Error are internal __wakeup classes restored through the graph */
$cache = UserCache\Cache::getPool('internal-wakeup-values');

$shared = [1.5, -0.0];
$magic = new SerdesMagic(['flags' => [true, false, null]]);
$trace = [
    'floats' => [3.25, -0.0, 1.0e-300, INF, -INF, 0.1 + 0.2, PHP_FLOAT_EPSILON],
    'nan' => NAN,
    'magic' => $magic,
    'magic-again' => $magic,
    'sleeper' => new SerdesSleeper(),
    'shared' => &$shared,
    'shared-again' => &$shared,
    'enum' => SerdesSuit::Hearts,
    'scalars' => [true, false, null, PHP_INT_MIN, ''],
    'empty' => [],
    7 => 'int-key',
    'nested' => ['deep' => ['deeper' => -1.25]],
];
$root = with_trace(new Exception('outer', 3, new Error('inner')), $trace);

ok('store', $cache->store('exception', $root));

SerdesMagic::$log = [];
$fetched = $cache->fetch('exception');
$fetchLog = SerdesMagic::$log;
SerdesMagic::$log = [];
$native = native_copy($root);
$nativeLog = SerdesMagic::$log;

ok('class', get_class($fetched) === Exception::class);
ok('native parity', serialize($fetched) === serialize($native));
ok('hook order matches native', $fetchLog === $nativeLog && $fetchLog === ['unserialize', 'wakeup']);
ok('previous', $fetched->getPrevious() instanceof Error && $fetched->getPrevious()->getMessage() === 'inner');
ok('message and code', $fetched->getMessage() === 'outer' && $fetched->getCode() === 3);

$fetchedTrace = $fetched->getTrace();
ok('float bits', $fetchedTrace['floats'] === $trace['floats']
    && fdiv(1.0, $fetchedTrace['floats'][1]) === -INF);
ok('nan', is_nan($fetchedTrace['nan']));
ok('object identity kept', $fetchedTrace['magic'] === $fetchedTrace['magic-again']);
ok('magic payload', $fetchedTrace['magic']->payload === ['payload' => ['flags' => [true, false, null]], 'ratio' => 0.75]);
ok('sleeper', $fetchedTrace['sleeper']->kept === 1
    && $fetchedTrace['sleeper']->weight === 2.5
    && $fetchedTrace['sleeper']->state === 'woken');
ok('enum', $fetchedTrace['enum'] === SerdesSuit::Hearts);
ok('scalars', $fetchedTrace['scalars'] === [true, false, null, PHP_INT_MIN, '']);
ok('int key', $fetchedTrace[7] === 'int-key');
ok('nested', $fetchedTrace['nested'] === ['deep' => ['deeper' => -1.25]]);

/* getTrace() returns a copy; mutate the fetched graph through reflection */
$traceProp = new ReflectionProperty(Exception::class, 'trace');
$held = $traceProp->getValue($fetched);
$held['shared'][] = 'appended';
ok('reference kept', $held['shared-again'] === [1.5, -0.0, 'appended']);
ok('reference detached from the stored source', $shared === [1.5, -0.0]);
$again = $cache->fetch('exception');
ok('fetches are isolated', $again !== $fetched
    && $traceProp->getValue($again)['shared'] === [1.5, -0.0]
    && $traceProp->getValue($again)['magic'] !== $held['magic']);

$error = with_trace(new Error('plain error', 9), ['args' => [1.5, [2 => 'x'], new SerdesSleeper()]]);
ok('Error root', $cache->store('error', $error)
    && serialize($cache->fetch('error')) === serialize(native_copy($error)));

/* Throwing nested hooks propagate from fetch() and fetchMultiple() and keep the entry */
$cache = UserCache\Cache::getPool('internal-wakeup-hooks');

class SerdesThrowingUnserialize
{
    public function __serialize(): array
    {
        return ['v' => 1.5];
    }

    public function __unserialize(array $data): void
    {
        throw new RuntimeException('__unserialize failed');
    }
}

class SerdesThrowingWakeup
{
    public float $v = 2.5;

    public function __sleep(): array
    {
        return ['v'];
    }

    public function __wakeup(): void
    {
        throw new RuntimeException('__wakeup failed');
    }
}

$cache->store('plain', 'fine');
foreach (['unserialize' => new SerdesThrowingUnserialize(), 'wakeup' => new SerdesThrowingWakeup()] as $key => $hooked) {
    $value = with_trace(new Exception('hooked'), [$hooked]);
    ok($key . ' store', $cache->store($key, $value));

    try {
        native_copy($value);
        $native = 'none';
    } catch (RuntimeException $e) {
        $native = $e->getMessage();
    }

    try {
        $cache->fetch($key);
        echo $key, " fetch: no exception\n";
    } catch (RuntimeException $e) {
        ok($key . ' fetch throws like unserialize()', $e->getMessage() === $native);
    }

    try {
        $cache->fetchMultiple(['plain', $key, 'missing']);
        echo $key, " fetchMultiple: no exception\n";
    } catch (RuntimeException $e) {
        ok($key . ' fetchMultiple throws like unserialize()', $e->getMessage() === $native);
    }

    ok($key . ' entry kept', $cache->has($key) && $cache->fetch('plain') === 'fine');
}

/* Unstorable members of an internal __wakeup object are refused without a partial store */
$cache = UserCache\Cache::getPool('internal-wakeup-refused');

class SerdesLazyTarget
{
    public int $x = 1;
}

class SerdesBadSleep
{
    public int $a = 1;

    public function __sleep()
    {
        return 5;
    }
}

class SerdesMissingMemberSleep
{
    public int $a = 1;

    public function __sleep(): array
    {
        return ['a', 'missing'];
    }
}

$resource = fopen(__FILE__, 'r');
$refused = [
    'closure' => static fn() => 1,
    'resource' => $resource,
    'lazy' => (new ReflectionClass(SerdesLazyTarget::class))->newLazyGhost(function ($object) {
        $object->x = 2;
    }),
    'weakref' => WeakReference::create($magic),
    'bad-sleep' => new SerdesBadSleep(),
];

foreach ($refused as $key => $member) {
    try {
        var_dump($cache->store($key, with_trace(new Exception('refused'), ['args' => [$member]])));
    } catch (TypeError $e) {
        echo $key, ': ', $e->getMessage(), "\n";
    }
    var_dump($cache->has($key));
}
fclose($resource);

$missing = with_trace(new Exception('missing'), [new SerdesMissingMemberSleep()]);
var_dump($cache->store('missing-member', $missing));
ok('missing member parity', @serialize($cache->fetch('missing-member')) === @serialize(native_copy($missing)));

/* Real call-stack arguments of internal and user exception classes */
$cache = UserCache\Cache::getPool('internal-wakeup-natural');

class SerdesAppException extends Exception
{
}

function serdes_throw_internal(float $ratio, object $box, array &$list, SerdesSuit $suit): never
{
    throw new RuntimeException('internal');
}

function serdes_throw_user(float $ratio, SerdesMagic $magic): never
{
    throw new SerdesAppException('user');
}

$list = [1, 2.5];
try {
    serdes_throw_internal(0.5, new ArrayObject(['k' => 1.25]), $list, SerdesSuit::Hearts);
} catch (RuntimeException $e) {
    $internal = $e;
}
try {
    serdes_throw_user(-0.5, new SerdesMagic([4.5]));
} catch (SerdesAppException $e) {
    $user = $e;
}

ok('internal exception with args', $cache->store('internal', $internal)
    && serialize($cache->fetch('internal')) === serialize(native_copy($internal)));
ok('user exception with args', $cache->store('user', $user)
    && serialize($cache->fetch('user')) === serialize(native_copy($user)));
?>
--EXPECTF--
store: OK
class: OK
native parity: OK
hook order matches native: OK
previous: OK
message and code: OK
float bits: OK
nan: OK
object identity kept: OK
magic payload: OK
sleeper: OK
enum: OK
scalars: OK
int key: OK
nested: OK
reference kept: OK
reference detached from the stored source: OK
fetches are isolated: OK
Error root: OK
unserialize store: OK
unserialize fetch throws like unserialize(): OK
unserialize fetchMultiple throws like unserialize(): OK
unserialize entry kept: OK
wakeup store: OK
wakeup fetch throws like unserialize(): OK
wakeup fetchMultiple throws like unserialize(): OK
wakeup entry kept: OK
closure: Closure objects cannot be stored in the user cache
bool(false)
resource: Resources cannot be stored in the user cache
bool(false)
lazy: Uninitialized lazy objects cannot be stored in the user cache
bool(false)
weakref: WeakReference objects cannot be stored in the user cache
bool(false)

Warning: UserCache\Cache::store(): SerdesBadSleep::__sleep() should return an array only containing the names of instance-variables to serialize in %s on line %d
bad-sleep: SerdesBadSleep::__sleep() did not return an array of member names; the object cannot be stored in the user cache
bool(false)

Warning: UserCache\Cache::store(): "missing" returned as member variable from __sleep() but does not exist in %s on line %d
bool(true)
missing member parity: OK
internal exception with args: OK
user exception with args: OK
