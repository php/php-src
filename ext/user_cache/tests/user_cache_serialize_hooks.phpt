--TEST--
UserCache\Cache: __serialize() and __unserialize() hooks follow the native serialization contract, including numeric property keys and the native unserialize() state after a fatal error
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=16M
error_reporting=E_ALL & ~E_DEPRECATED
--FILE--
<?php
function ok(string $label, bool $cond): void
{
    echo $label, ': ', $cond ? 'OK' : 'FAIL', "\n";
}

/* Paired __serialize()/__unserialize() rebuild state that cannot be copied */
$cache = UserCache\Cache::getPool('serialize-hooks-paired');

class ContractClosureHolder
{
    private ?Closure $formatter;

    public function __construct(private string $prefix)
    {
        $this->formatter = fn (string $v): string => $this->prefix . ':' . $v;
    }

    public function __serialize(): array
    {
        return ['prefix' => $this->prefix];
    }

    public function __unserialize(array $data): void
    {
        $this->prefix = $data['prefix'];
        $this->formatter = fn (string $v): string => $this->prefix . ':' . $v;
    }

    public function format(string $v): string
    {
        return ($this->formatter)($v);
    }
}

$holder = new ContractClosureHolder('log');
var_dump($cache->store('closure-holder', $holder));
var_dump($cache->fetch('closure-holder')->format('entry'));

class ContractIndexedCollection
{
    /** @var array<int, array{id: int, name: string}> */
    private array $items;
    /** @var array<string, int> */
    private array $indexByName = [];

    public function __construct(array $items)
    {
        $this->items = $items;
        $this->rebuildIndex();
    }

    public function __serialize(): array
    {
        return ['items' => $this->items];
    }

    public function __unserialize(array $data): void
    {
        $this->items = $data['items'];
        $this->rebuildIndex();
    }

    private function rebuildIndex(): void
    {
        $this->indexByName = [];
        foreach ($this->items as $position => $item) {
            $this->indexByName[$item['name']] = $position;
        }
    }

    public function findByName(string $name): ?array
    {
        $position = $this->indexByName[$name] ?? null;

        return $position === null ? null : $this->items[$position];
    }
}

$collection = new ContractIndexedCollection([
    ['id' => 10, 'name' => 'alpha'],
    ['id' => 20, 'name' => 'beta'],
]);
var_dump($cache->store('indexed', $collection));
var_dump($cache->fetch('indexed')->findByName('beta')['id']);

class ContractPackedMatrix
{
    /** @var array<int, array<int, int>> */
    private array $rows;

    public function __construct(array $rows)
    {
        $this->rows = $rows;
    }

    public function __serialize(): array
    {
        return ['packed' => implode(';', array_map(static fn (array $r): string => implode(',', $r), $this->rows))];
    }

    public function __unserialize(array $data): void
    {
        $this->rows = array_map(
            static fn (string $r): array => array_map('intval', explode(',', $r)),
            explode(';', $data['packed'])
        );
    }

    public function cell(int $row, int $column): int
    {
        return $this->rows[$row][$column];
    }
}

var_dump($cache->store('matrix', new ContractPackedMatrix([[1, 2], [3, 4]])));
var_dump($cache->fetch('matrix')->cell(1, 0));

class ContractParent
{
    private array $secret = [];

    public function __serialize(): array
    {
        return ['secret' => $this->secret];
    }

    public function __unserialize(array $data): void
    {
        $this->secret = $data['secret'];
    }

    public function addSecret(string $v): void
    {
        $this->secret[] = $v;
    }

    public function secretCount(): int
    {
        return count($this->secret);
    }
}

class ContractChild extends ContractParent
{
    public string $label = 'child';
}

$child = new ContractChild();
$child->addSecret('a');
$child->addSecret('b');
var_dump($cache->store('child', $child));
$fetchedChild = $cache->fetch('child');
var_dump($fetchedChild instanceof ContractChild, $fetchedChild->secretCount(), $fetchedChild->label);

class ContractReadonly
{
    public function __construct(public readonly string $token)
    {
    }

    public function __serialize(): array
    {
        return ['token' => $this->token];
    }

    public function __unserialize(array $data): void
    {
        $this->token = $data['token'];
    }
}

var_dump($cache->store('ro', new ContractReadonly('tok-1')));
var_dump($cache->fetch('ro')->token);

class ContractSelf
{
    public ?ContractSelf $self = null;

    public function __construct(public int $v = 1)
    {
    }

    public function __serialize(): array
    {
        return ['self' => $this, 'v' => $this->v];
    }

    public function __unserialize(array $data): void
    {
        $this->self = $data['self'];
        $this->v = $data['v'];
    }
}

var_dump($cache->store('self', new ContractSelf()));
$fetchedSelf = $cache->fetch('self');
var_dump($fetchedSelf->self === $fetchedSelf, $fetchedSelf->v);

$nested = [
    'holders' => [new ContractClosureHolder('a'), new ContractClosureHolder('b')],
    'plain' => ['x' => 1],
];
var_dump($cache->store('nested', $nested));
$fetchedNested = $cache->fetch('nested');
var_dump($fetchedNested['holders'][1]->format('n'), $fetchedNested['plain']['x']);

class ContractBadReturn
{
    public function __serialize()
    {
        return 'not-an-array';
    }

    public function __unserialize(array $data): void
    {
    }
}

try {
    $cache->store('bad', new ContractBadReturn());
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

/* Release builds do not collect cycles at shutdown. */
unset($holder, $collection, $child, $fetchedChild, $fetchedSelf, $nested, $fetchedNested);
gc_collect_cycles();

/* __serialize() without __unserialize() restores by property assignment */
$cache = UserCache\Cache::getPool('serialize-hooks-serialize-only');

class SerializeOnly
{
    public int $kept = 0;
    public string $dropped = 'default';

    public function __construct(int $kept)
    {
        $this->kept = $kept;
        $this->dropped = 'set';
    }

    public function __serialize(): array
    {
        return ['kept' => $this->kept];
    }
}

$value = new SerializeOnly(5);
$cache->store('basic', $value);
$fetched = $cache->fetch('basic');
ok('basic instanceof', $fetched instanceof SerializeOnly);
ok('basic kept restored', $fetched->kept === 5);
ok('basic omitted reverts to default', $fetched->dropped === 'default');
ok('basic parity', serialize($fetched) === serialize($value));

class SerializeOnlyWakeful
{
    public static int $wakeupCalls = 0;
    public int $v = 0;

    public function __construct(int $v)
    {
        $this->v = $v;
    }

    public function __serialize(): array
    {
        return ['v' => $this->v];
    }

    public function __wakeup(): void
    {
        self::$wakeupCalls++;
    }
}

SerializeOnlyWakeful::$wakeupCalls = 0;
$cache->store('wakeful', new SerializeOnlyWakeful(9));
$first = $cache->fetch('wakeful');
$second = $cache->fetch('wakeful');
ok('wakeful value', $first->v === 9 && $second->v === 9);
ok('wakeful __wakeup per fetch', SerializeOnlyWakeful::$wakeupCalls === 2);
ok('wakeful distinct instances', $first !== $second);

class SerializeOnlyRich
{
    public int $num;
    private string $secret;
    public ?object $child;

    public function __construct()
    {
        $this->num = 1;
        $this->secret = 'hidden';
        $this->child = null;
    }

    public function __serialize(): array
    {
        return [
            'num' => 42,
            "\0" . self::class . "\0secret" => 'restored',
            'child' => (object) ['deep' => [1, 2, 3]],
        ];
    }

    public function reveal(): string
    {
        return $this->secret;
    }
}

$rich = new SerializeOnlyRich();
$cache->store('rich', $rich);
$fetchedRich = $cache->fetch('rich');
ok('rich typed', $fetchedRich->num === 42);
ok('rich private', $fetchedRich->reveal() === 'restored');
ok('rich nested object', $fetchedRich->child instanceof stdClass && $fetchedRich->child->deep === [1, 2, 3]);
ok('rich parity', serialize($fetchedRich) === serialize($rich));

$graph = ['a' => new SerializeOnly(1), 'list' => [new SerializeOnly(2), new SerializeOnly(3)]];
$cache->store('graph', $graph);
$fetchedGraph = $cache->fetch('graph');
ok('graph values', $fetchedGraph['a']->kept === 1
    && $fetchedGraph['list'][0]->kept === 2
    && $fetchedGraph['list'][1]->kept === 3);
ok('graph parity', serialize($fetchedGraph) === serialize($graph));

#[\AllowDynamicProperties]
class SerializeOnlyIntKeys
{
    public int $declared = 1;

    public function __serialize(): array
    {
        return ['declared' => 7, 42 => 'answer', 'label' => 'x'];
    }
}

$intKeys = new SerializeOnlyIntKeys();
$cache->store('intkeys', $intKeys);
$fetchedIntKeys = $cache->fetch('intkeys');
ok('integer key parity', serialize($fetchedIntKeys) === serialize(unserialize(serialize($intKeys))));
ok('integer key property', $fetchedIntKeys->declared === 7 && $fetchedIntKeys->{42} === 'answer');

class SerializeOnlyResource
{
    public $handle;

    public function __serialize(): array
    {
        return ['handle' => $this->handle];
    }
}

$withResource = new SerializeOnlyResource();
$withResource->handle = fopen(__FILE__, 'r');
try {
    $cache->store('resource', $withResource);
    ok('resource rejected', false);
} catch (TypeError $e) {
    ok('resource rejected', true);
}
fclose($withResource->handle);

/* __unserialize() without __serialize() receives the property table */
$cache = UserCache\Cache::getPool('serialize-hooks-unserialize-only');

class UnserializeOnlyContract
{
    public int $value = 7;
    public int $derived = 0;
    public static int $calls = 0;

    public function __unserialize(array $data): void
    {
        self::$calls++;
        $this->value = $data['value'];
        $this->derived = $this->value * 3;
    }
}

class SleepAndUnserializeContract
{
    public int $keep = 5;
    public int $drop = 99;
    public bool $wakeup = false;
    public array $keys = [];
    public static array $events = [];

    public function __sleep(): array
    {
        self::$events[] = 'sleep';

        return ['keep'];
    }

    public function __wakeup(): void
    {
        self::$events[] = 'wakeup';
        $this->wakeup = true;
    }

    public function __unserialize(array $data): void
    {
        self::$events[] = 'unserialize';
        $this->keys = array_keys($data);
        $this->keep = $data['keep'];
        $this->drop = -1;
    }
}

UnserializeOnlyContract::$calls = 0;
SleepAndUnserializeContract::$events = [];

var_dump($cache->store('one', new UnserializeOnlyContract()));
$one = $cache->fetch('one');
var_dump($one->value, $one->derived, UnserializeOnlyContract::$calls);

var_dump($cache->store('two', new SleepAndUnserializeContract()));
$two = $cache->fetch('two');
var_dump($two->keep, $two->drop, $two->wakeup, $two->keys, SleepAndUnserializeContract::$events);

#[AllowDynamicProperties]
class Plain
{
    public $got;

    public function __unserialize(array $data): void
    {
        $this->got = [gettype(array_key_first($data)), isset($data[7]), $data[7] ?? null];
    }
}

#[AllowDynamicProperties]
class Sleeps
{
    public $got;

    public function __sleep(): array
    {
        return ['7'];
    }

    public function __unserialize(array $data): void
    {
        $this->got = [gettype(array_key_first($data)), isset($data[7]), $data[7] ?? null];
    }
}

function unserialize_route_numeric_property_keys(): void
{
    $cache = UserCache\Cache::getPool('unserialize-route-numeric-keys');
    foreach ([new Plain, new Sleeps] as $object) {
        unset($object->got);
        $object->{'7'} = 'seven';
        $cache->store('k', [$object]);
        var_dump($cache->fetch('k')[0]->got === unserialize(serialize([$object]))[0]->got, $cache->fetch('k')[0]->got);
    }
}

echo "\nunserialize route numeric property keys:\n";
unserialize_route_numeric_property_keys();

class FatalInCacheUnserialize implements Serializable
{
    public function serialize()
    {
        return 'payload';
    }

    public function unserialize($data)
    {
        unserialize('a:1:{i:0;O:8:"stdClass":0:{}}');
        eval('class SerdesUnserializeBailoutRedeclared {} class SerdesUnserializeBailoutRedeclared {}');
    }
}

function serdes_unserialize_bailout(): void
{
    register_shutdown_function(function () {
        echo "shutdown\n";

        $restored = unserialize('a:2:{i:0;O:8:"stdClass":0:{}i:1;r:2;}');
        var_dump($restored[0] === $restored[1]);
    });

    $cache = UserCache\Cache::getPool('serdes-unserialize-bailout');
    var_dump($cache->store('key', new FatalInCacheUnserialize()));
    $cache->fetch('key');
    echo "unreachable\n";
}

echo "\nserdes unserialize bailout:\n";
serdes_unserialize_bailout();
?>
--EXPECTF--
bool(true)
string(9) "log:entry"
bool(true)
int(20)
bool(true)
int(3)
bool(true)
bool(true)
int(2)
string(5) "child"
bool(true)
string(5) "tok-1"
bool(true)
bool(true)
int(1)
bool(true)
string(3) "b:n"
int(1)
ContractBadReturn::__serialize() must return an array
basic instanceof: OK
basic kept restored: OK
basic omitted reverts to default: OK
basic parity: OK
wakeful value: OK
wakeful __wakeup per fetch: OK
wakeful distinct instances: OK
rich typed: OK
rich private: OK
rich nested object: OK
rich parity: OK
graph values: OK
graph parity: OK
integer key parity: OK
integer key property: OK
resource rejected: OK
bool(true)
int(7)
int(21)
int(1)
bool(true)
int(5)
int(-1)
bool(false)
array(1) {
  [0]=>
  string(4) "keep"
}
array(2) {
  [0]=>
  string(5) "sleep"
  [1]=>
  string(11) "unserialize"
}

unserialize route numeric property keys:
bool(true)
array(3) {
  [0]=>
  string(7) "integer"
  [1]=>
  bool(true)
  [2]=>
  string(5) "seven"
}
bool(true)
array(3) {
  [0]=>
  string(7) "integer"
  [1]=>
  bool(true)
  [2]=>
  string(5) "seven"
}

serdes unserialize bailout:
bool(true)

Fatal error: Cannot redeclare class SerdesUnserializeBailoutRedeclared (previously declared in %s) in %s on line 1
shutdown
bool(true)
