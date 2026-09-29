--TEST--
UserCache\Cache: restoring objects never calls write-property hooks, refuses classes that native unserialize() refuses, restores class names that now alias internal classes, properties that became static, stale enum cases and safe-direct internal properties like unserialize(), and isolates autoloaders run by a safe-direct restore
--EXTENSIONS--
pcntl
--INI--
user_cache.enable=1
user_cache.enable_cli=1
--FILE--
<?php
use UserCache\Cache;

#[AllowDynamicProperties]
class SettingBox
{
    public function __set($name, $value)
    {
        $this->$name = $value . '!';
    }
}

#[AllowDynamicProperties]
class PropsArrayObject extends ArrayObject
{
}

class StampedDate extends DateTimeImmutable
{
    public $ctime = 'default';
    public $timezone_type = 'user';
}

class TaggedInterval extends DateInterval
{
    public $rel = 'default';
}

class TaggedPeriod extends DatePeriod
{
    public $start_ce = 'default';
}

class Outer implements Serializable
{
    public static ?ArrayObject $fetched = null;
    public static ?array $autoloaded = null;

    public function serialize(): string
    {
        return '';
    }

    public function unserialize(string $data): void
    {
        self::$fetched = UserCache\Cache::getPool('safe-direct-autoload-lock')->fetch('array-object');
    }

    public function __serialize(): array
    {
        return [];
    }

    public function __unserialize(array $data): void
    {
    }
}

/* Values are decoded by a fresh process so no request-local copy is reused. */
function fetch_in_child(string $key, callable $check): void
{
    $pid = pcntl_fork();
    if ($pid === 0) {
        $check(Cache::getPool('restore-parity')->fetch($key, 'default'));
        exit(0);
    }

    pcntl_waitpid($pid, $status);
}

function outcome(Closure $restore): string
{
    try {
        return get_debug_type($restore());
    } catch (Throwable $e) {
        return $e::class . ': ' . $e->getMessage();
    }
}

/* Store bare enums and enums nested in exception trace arguments. */
function user_cache_stale_enum_capture($case): RuntimeException
{
    return new RuntimeException('captured');
}

function restore_internal_alias(): void
{
    $cache = Cache::getPool('restore-internal-alias');
    $cache->clear();

    $pid = pcntl_fork();
    if ($pid === 0) {
        eval('class AliasPlain {} class AliasSleeps { public function __sleep() { return []; } } class AliasNested {} class AliasArray { public $a = 1; } class AliasIterator { public $a = 1; }');
        $child = Cache::getPool('restore-internal-alias');
        $values = [
            'plain' => new AliasPlain(),
            'sleeps' => ['x' => new AliasSleeps()],
            'nested' => ['list' => [1, 2], 'obj' => new AliasNested()],
            'array' => new AliasArray(),
            'iterator' => new AliasIterator(),
        ];
        foreach ($values as $key => $value) {
            $child->store($key, $value);
            $child->store("native-$key", serialize($value));
        }
        exit(0);
    }
    pcntl_waitpid($pid, $status);

    class_alias(Random\Randomizer::class, 'AliasPlain');
    class_alias(Random\Randomizer::class, 'AliasSleeps');
    class_alias(Random\Randomizer::class, 'AliasNested');
    class_alias(ArrayObject::class, 'AliasArray');
    class_alias(IteratorIterator::class, 'AliasIterator');

    foreach (['plain', 'sleeps', 'nested', 'array'] as $key) {
        $fetched = outcome(fn () => $cache->fetch($key, 'refused'));
        $native = outcome(fn () => unserialize($cache->fetch("native-$key")));
        echo $key, ': ', $fetched, $fetched === $native ? ' (same as unserialize())' : " (unserialize(): $native)", "\n";
    }

    var_dump($cache->fetch('iterator', 'refused'), $cache->has('iterator'));
}

function restore_static_property_rename(): void
{
    $cache = Cache::getPool('restore-static-property-rename');
    $cache->clear();

    $pid = pcntl_fork();
    if ($pid === 0) {
        eval('class BecameStatic { public $a = "v"; }');
        $object = new BecameStatic();
        file_put_contents(__DIR__ . '/user_cache_object_restore_native_parity_static_property.ser', serialize($object));
        Cache::getPool('restore-static-property-rename')->store('k', $object);
        exit(0);
    }
    pcntl_waitpid($pid, $status);

    eval('class BecameStatic { public static $a = "s"; }');

    set_error_handler(function (int $errno, string $message): bool {
        echo 'error: ', $message, "\n";

        return true;
    });

    $fetched = $cache->fetch('k');
    $native = unserialize(file_get_contents(__DIR__ . '/user_cache_object_restore_native_parity_static_property.ser'));
    var_dump($fetched, $fetched == $native, BecameStatic::$a);

    restore_error_handler();
}

function stale_enum_case(): void
{
    $cache = UserCache\Cache::getPool('stale-enum-case');
    $cache->clear();

    $names = ['missing', 'const', 'kept', 'backed'];

    $pid = pcntl_fork();
    if ($pid === 0) {
        /* Enum definitions in effect when the entries are stored. */
        enum UserCacheStaleEnumMissing { case Active; case Gone; }
        enum UserCacheStaleEnumConst { case Active; case Renamed; }
        enum UserCacheStaleEnumKept { case Active; }
        enum UserCacheStaleEnumBacked: string { case Active = 'active'; }

        $cases = [
            'missing' => UserCacheStaleEnumMissing::Gone,
            'const'   => UserCacheStaleEnumConst::Renamed,
            'kept'    => UserCacheStaleEnumKept::Active,
            'backed'  => UserCacheStaleEnumBacked::Active,
        ];
        $ok = true;
        foreach ($cases as $name => $case) {
            $ok = $cache->store("$name-graph", $case) && $ok;
            $ok = $cache->store("$name-trace", user_cache_stale_enum_capture($case)) && $ok;
        }
        exit($ok ? 0 : 1);
    }

    pcntl_waitpid($pid, $status);
    var_dump(pcntl_wifexited($status) && pcntl_wexitstatus($status) === 0);

    /* Enum definitions in effect when the entries are fetched (stale). */
    enum UserCacheStaleEnumMissing { case Active; }
    enum UserCacheStaleEnumConst { case Active; const Renamed = 'renamed'; }
    enum UserCacheStaleEnumKept { case Active; }
    enum UserCacheStaleEnumBacked: string { case Active = 'active'; }

    foreach ($names as $name) {
        echo "$name-graph: ";
        var_dump($cache->fetch("$name-graph", 'default'));
        echo "$name-graph has: ";
        var_dump($cache->has("$name-graph"));

        echo "$name-trace: ";
        $value = $cache->fetch("$name-trace", 'default');
        if ($value instanceof RuntimeException) {
            var_dump($value->getTrace()[0]['args'][0]);
        } else {
            var_dump($value);
        }
        echo "$name-trace has: ";
        var_dump($cache->has("$name-trace"));
    }
}

function safe_direct_internal_properties(): void
{
    $interval = new DateInterval('P1D');
    json_encode($interval);
    $interval->d = 5;

    $date = new StampedDate('2020-01-02 03:04:05', new DateTimeZone('Europe/Paris'));
    $date->ctime = 'changed';

    $tagged = new TaggedInterval('P2D');
    $tagged->rel = 'changed';

    $period = new TaggedPeriod(new DateTime('2020-01-01'), new DateInterval('P1D'), 2);
    $period->start_ce = 'changed';

    $values = ['interval' => $interval, 'date' => $date, 'tagged' => $tagged, 'period' => $period];
    $cache = Cache::getPool('safe-direct-internal-properties');
    foreach ($values as $key => $value) {
        $cache->store($key, $value);
    }

    /* A fresh process decodes the stored state instead of a request-local copy. */
    $pid = pcntl_fork();
    if ($pid === 0) {
        $cache = Cache::getPool('safe-direct-internal-properties');
        foreach ($values as $key => $value) {
            $fetched = $cache->fetch($key);
            echo $key, ': ', serialize($fetched) === serialize(unserialize(serialize($value))) ? 'same as unserialize()' : 'DIFFERENT', "\n";
        }
        var_dump($cache->fetch('interval')->d, $cache->fetch('date')->ctime, $cache->fetch('tagged')->rel, $cache->fetch('period')->start_ce);
        exit(0);
    }
    pcntl_waitpid($pid, $status);
}

function safe_direct_autoload_serialize_lock(): void
{
    $cache = UserCache\Cache::getPool('safe-direct-autoload-lock');
    $cache->delete('array-object');

    $pid = pcntl_fork();
    if ($pid === 0) {
        eval('class LazyIterator extends ArrayIterator {}');
        $value = new ArrayObject([1, 2]);
        $value->setIteratorClass('LazyIterator');
        $cache->store('array-object', $value);
        exit(0);
    }
    pcntl_waitpid($pid, $status);

    spl_autoload_register(function (string $class): void {
        if ($class === 'LazyIterator') {
            Outer::$autoloaded = unserialize('a:2:{i:0;O:8:"stdClass":1:{s:3:"tag";s:5:"inner";}i:1;r:2;}');
            eval('class LazyIterator extends ArrayIterator {}');
        }
    });

    $outer = unserialize('a:2:{i:0;O:8:"stdClass":1:{s:3:"tag";s:5:"outer";}i:1;C:5:"Outer":0:{}}');
    var_dump($outer[0]->tag, get_class($outer[1]));
    var_dump(Outer::$autoloaded[0]->tag, Outer::$autoloaded[1]->tag);
    var_dump(Outer::$fetched->getIteratorClass(), Outer::$fetched->getArrayCopy());
}

$cache = Cache::getPool('restore-parity');
$cache->clear();

$box = new SettingBox();
$box->name = 'a';
var_dump($box->name, $cache->store('box', $box));
fetch_in_child('box', function ($fetched) use ($box): void {
    var_dump($fetched->name, $fetched == unserialize(serialize($box)));
});

$array = new PropsArrayObject(['k' => 'v']);
$array->extra = 1;
$array->setFlags(ArrayObject::ARRAY_AS_PROPS);
var_dump($cache->store('array', $array));
fetch_in_child('array', function ($fetched) use ($array): void {
    var_dump($fetched->getArrayCopy(), serialize($fetched) === serialize(unserialize(serialize($array))));
});

try {
    $cache->store('anonymous', new class('2020-01-01') extends DateTimeImmutable {});
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

$pid = pcntl_fork();
if ($pid === 0) {
    eval('class RestoreParityAliased {}');
    Cache::getPool('restore-parity')->store('aliased', new RestoreParityAliased());
    exit(0);
}
pcntl_waitpid($pid, $status);
class_alias('Closure', 'RestoreParityAliased');
var_dump($cache->fetch('aliased', 'refused'));

echo "\ninternal alias:\n";
restore_internal_alias();

echo "\nstatic property rename:\n";
restore_static_property_rename();

echo "\nstale enum case:\n";
stale_enum_case();

echo "\nsafe-direct internal properties:\n";
safe_direct_internal_properties();

echo "\nsafe-direct autoload serialize lock:\n";
safe_direct_autoload_serialize_lock();
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/user_cache_object_restore_native_parity_static_property.ser');
?>
--EXPECTF--
string(2) "a!"
bool(true)
string(2) "a!"
bool(true)
bool(true)
array(1) {
  ["k"]=>
  string(1) "v"
}
bool(true)
DateTimeImmutable@anonymous objects cannot be stored in the user cache
string(7) "refused"

internal alias:
plain: Exception: Invalid serialization data for Random\Randomizer object (same as unserialize())
sleeps: Exception: Invalid serialization data for Random\Randomizer object (same as unserialize())
nested: Exception: Invalid serialization data for Random\Randomizer object (same as unserialize())
array: UnexpectedValueException: Incomplete or ill-typed serialization data (same as unserialize())
string(7) "refused"
bool(false)

static property rename:
object(BecameStatic)#%d (1) {
  ["a"]=>
  string(1) "v"
}
bool(true)
string(1) "s"

stale enum case:
bool(true)
missing-graph: string(7) "default"
missing-graph has: bool(false)
missing-trace: string(7) "default"
missing-trace has: bool(false)
const-graph: string(7) "default"
const-graph has: bool(false)
const-trace: string(7) "default"
const-trace has: bool(false)
kept-graph: enum(UserCacheStaleEnumKept::Active)
kept-graph has: bool(true)
kept-trace: enum(UserCacheStaleEnumKept::Active)
kept-trace has: bool(true)
backed-graph: enum(UserCacheStaleEnumBacked::Active)
backed-graph has: bool(true)
backed-trace: enum(UserCacheStaleEnumBacked::Active)
backed-trace has: bool(true)

safe-direct internal properties:
interval: same as unserialize()
date: same as unserialize()
tagged: same as unserialize()
period: same as unserialize()
int(5)
string(7) "changed"
string(7) "changed"
string(7) "changed"

safe-direct autoload serialize lock:
string(5) "outer"
string(5) "Outer"
string(5) "inner"
string(5) "inner"
string(12) "LazyIterator"
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
