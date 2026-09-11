--TEST--
UserCache\Cache: classes that cannot round-trip without __serialize()/__unserialize()
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
$cache = UserCache\Cache::getPool('magic-contract');

/* The serialization contract replaces an unstorable closure. */
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

/* Snapshot hooks run once per store; restore hooks run on every fetch. */
class ContractCounter
{
    public static int $serializeCalls = 0;
    public static int $unserializeCalls = 0;

    public function __construct(private int $value = 7)
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

var_dump($cache->store('counter', new ContractCounter()));
var_dump(ContractCounter::$serializeCalls);
$cache->fetch('counter');
$cache->fetch('counter');
var_dump(ContractCounter::$unserializeCalls);

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

unset($holder, $collection, $child, $fetchedChild, $fetchedSelf, $nested, $fetchedNested);
gc_collect_cycles();
?>
--EXPECT--
bool(true)
string(9) "log:entry"
bool(true)
int(20)
bool(true)
int(3)
bool(true)
int(1)
int(2)
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
