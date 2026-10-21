--TEST--
UserCache\Cache: every serializable value class round-trips with identity, readonly state and float bits intact
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=32M
--FILE--
<?php
function ok(string $label, bool $cond): void
{
    echo $label, ': ', $cond ? 'OK' : 'FAIL', "\n";
}

/* Value matrix: serialize parity or an explicit verifier per value class */
$cache = UserCache\Cache::getPool('value-matrix');

function check(UserCache\Cache $cache, string $label, mixed $value, ?callable $verify = null): void
{
    static $i = 0;

    $key = 'matrix-' . $i++;
    if (!$cache->store($key, $value)) {
        echo $label, ": STORE FAILED\n";
        return;
    }

    $fetched = $cache->fetch($key);
    $ok = $verify !== null
        ? $verify($value, $fetched)
        : serialize($fetched) === serialize($value);
    echo $label, ': ', $ok ? 'OK' : 'MISMATCH', "\n";

    /* Release builds do not collect cycles at shutdown. */
    unset($fetched);
    gc_collect_cycles();
}

check($cache, 'null', null);
check($cache, 'bool', true);
check($cache, 'int', PHP_INT_MAX);
check($cache, 'negative int', PHP_INT_MIN);
check($cache, 'float', 1.5);
check($cache, 'float INF', INF);
check($cache, 'float NAN', NAN);
check($cache, 'empty string', '');
check($cache, 'binary string', "a\0b\xff\xfe");
check($cache, 'unicode string', 'こんにちは🌏');

check($cache, 'empty array', []);
check($cache, 'packed array', range(1, 64));
check($cache, 'hashed array', ['a' => 1, 'b' => ['c' => 2.5, 'd' => null], 10 => 'x']);
check($cache, 'binary keys', ["k\0ey" => 1, '0' => 2]);
$deep = 'leaf';
for ($d = 0; $d < 32; $d++) {
    $deep = ['level' => $d, 'inner' => $deep];
}
check($cache, 'deep array', $deep);
$aliased = ['v' => 1];
$aliased['alias'] = &$aliased['v'];
check($cache, 'array with references', $aliased);
$selfref = ['x' => 1];
$selfref['self'] = &$selfref;
check($cache, 'self-referencing array', $selfref, static function ($orig, $fetched) {
    /* Argument copying detaches the outer reference; inspect the cycle directly. */
    return $fetched['self']['self']['self']['x'] === 1 && is_array($fetched['self']['self']);
});

check($cache, 'stdClass', (object) ['a' => 1, 'nested' => (object) ['b' => 2]]);

class CoveragePlain
{
    public int $pub = 1;
    protected string $prot = 'p';
    private array $priv = ['x'];
}
check($cache, 'plain class with visibility', new CoveragePlain());

#[\AllowDynamicProperties]
class CoverageDynamic
{
    public int $declared = 1;
}
$dynamic = new CoverageDynamic();
$dynamic->added = 'dynamic';
check($cache, 'dynamic properties', $dynamic);

class CoverageReadonly
{
    public function __construct(public readonly string $name) {}
}
check($cache, 'readonly property', new CoverageReadonly('ro'));

class CoverageMagicSerialize
{
    public function __construct(private array $state) {}
    public function __serialize(): array { return ['state' => $this->state]; }
    public function __unserialize(array $data): void { $this->state = $data['state']; }
}
check($cache, '__serialize/__unserialize', new CoverageMagicSerialize(['k' => [1, 2]]));

class CoverageSleep
{
    public int $kept = 5;
    public string $dropped = 'gone';
    public function __sleep(): array { return ['kept']; }
    public function __wakeup(): void {}
}
check($cache, '__sleep/__wakeup', new CoverageSleep());

$shared = new stdClass();
$shared->tag = 'shared';
check($cache, 'shared object identity', [$shared, $shared], static function ($orig, $fetched) {
    return $fetched[0] === $fetched[1] && $fetched[0]->tag === 'shared';
});

class CoverageNode
{
    public ?CoverageNode $parent = null;
    public array $children = [];
    public function __construct(public string $id) {}
}
$root = new CoverageNode('root');
$child = new CoverageNode('child');
$child->parent = $root;
$root->children[] = $child;
check($cache, 'cyclic object graph', $root, static function ($orig, $fetched) {
    return $fetched->children[0]->parent === $fetched && $fetched->children[0]->id === 'child';
});

$selfobj = new CoverageNode('self');
$selfobj->parent = $selfobj;
check($cache, 'self-referencing object', $selfobj, static function ($orig, $fetched) {
    return $fetched->parent === $fetched;
});

enum CoverageSuit: string
{
    case Hearts = 'h';
    case Spades = 's';
}
check($cache, 'backed enum case', CoverageSuit::Spades, static function ($orig, $fetched) {
    return $fetched === CoverageSuit::Spades;
});
check($cache, 'enum inside array', ['suit' => CoverageSuit::Hearts], static function ($orig, $fetched) {
    return $fetched['suit'] === CoverageSuit::Hearts;
});

check($cache, 'DateTimeImmutable', new DateTimeImmutable('2026-07-02 12:00:00', new DateTimeZone('Asia/Tokyo')));
check($cache, 'DateInterval', new DateInterval('P1DT2H'));
check($cache, 'ArrayObject', new ArrayObject(['a' => 1, 'b' => 2]));
check($cache, 'SplStack', (static function () {
    $s = new SplStack();
    $s->push('one');
    $s->push('two');
    return $s;
})());
check($cache, 'SplFixedArray', SplFixedArray::fromArray([1, 'two', 3.0]));
check($cache, 'SplObjectStorage', (static function () {
    $s = new SplObjectStorage();
    $o = new stdClass();
    $o->v = 1;
    $s[$o] = 'data';
    return $s;
})(), static function ($orig, $fetched) {
    if (!$fetched instanceof SplObjectStorage || count($fetched) !== 1) {
        return false;
    }
    $fetched->rewind();
    return $fetched->current()->v === 1 && $fetched->getInfo() === 'data';
});
check($cache, 'Randomizer engine object', new Random\Randomizer(new Random\Engine\Xoshiro256StarStar(1234)), static function ($orig, $fetched) {
    return $fetched->getInt(0, PHP_INT_MAX) === $orig->getInt(0, PHP_INT_MAX);
});

check($cache, 'mixed payload', [
    'config' => ['ttl' => 300, 'flags' => [true, false]],
    'entity' => new CoverageMagicSerialize(['id' => 7]),
    'sleeper' => new CoverageSleep(),
    'when' => new DateTimeImmutable('2026-01-01', new DateTimeZone('UTC')),
    'suit' => CoverageSuit::Hearts,
], static function ($orig, $fetched) {
    return $fetched['config']['ttl'] === 300
        && $fetched['entity'] instanceof CoverageMagicSerialize
        && $fetched['sleeper'] instanceof CoverageSleep
        && $fetched['when'] instanceof DateTimeImmutable
        && $fetched['suit'] === CoverageSuit::Hearts;
});

/* Release builds do not collect cycles at shutdown. */
unset($deep, $aliased, $selfref, $dynamic, $shared, $root, $child, $selfobj);
gc_collect_cycles();

/* Pure enum cases restore as the singleton */
$cache = UserCache\Cache::getPool('value-matrix-pure-enum');

enum PureSuit
{
    case Hearts;
    case Spades;
}

var_dump($cache->store('single', PureSuit::Hearts));
var_dump($cache->fetch('single') === PureSuit::Hearts);

var_dump($cache->store('list', [PureSuit::Hearts, PureSuit::Spades, PureSuit::Hearts]));
$fetched = $cache->fetch('list');
var_dump($fetched[0] === PureSuit::Hearts);
var_dump($fetched[1] === PureSuit::Spades);
var_dump($fetched[2] === PureSuit::Hearts);

/* Readonly flag is re-established on fetched objects, including inherited properties */
$cache = UserCache\Cache::getPool('value-matrix-readonly');

class UserCacheReadonlyBase
{
    public function __construct(
        public readonly string $baseName,
        protected readonly int $baseCount,
    ) {}

    public function describe(): string
    {
        return $this->baseName . ':' . $this->baseCount;
    }
}

class UserCacheReadonlyChild extends UserCacheReadonlyBase
{
    public function __construct(
        string $baseName,
        int $baseCount,
        public readonly array $items,
    ) {
        parent::__construct($baseName, $baseCount);
    }
}

var_dump($cache->store('child', new UserCacheReadonlyChild('alpha', 7, ['a', 'b'])));

$fetched = $cache->fetch('child');
var_dump($fetched instanceof UserCacheReadonlyChild);
var_dump($fetched->describe());
var_dump($fetched->baseName);
var_dump($fetched->items);

try {
    $fetched->baseName = 'changed';
} catch (Error $e) {
    echo get_class($e), "\n";
}

/* Floats round-trip bit-exactly */
$cache = UserCache\Cache::getPool('value-matrix-floats');

$floats = [
    'positive zero' => 0.0,
    'negative zero' => -0.0,
    'epsilon' => PHP_FLOAT_EPSILON,
    'float min' => PHP_FLOAT_MIN,
    'float max' => PHP_FLOAT_MAX,
    'smallest subnormal' => 4.9e-324,
    'one third' => 1 / 3,
    'point one plus point two' => 0.1 + 0.2,
    'large' => 1.7e308,
    'small negative' => -2.2250738585072014e-308,
];

foreach ($floats as $label => $value) {
    $cache->store('f', $value);
    ok($label, pack('d', $cache->fetch('f')) === pack('d', $value));
}

$cache->store('negzero', -0.0);
ok('negative zero sign bit', fdiv(1.0, $cache->fetch('negzero')) === -INF);

foreach (['inf' => INF, '-inf' => -INF] as $label => $value) {
    $cache->store('nf', $value);
    ok($label, $cache->fetch('nf') === $value);
}
$cache->store('nan', NAN);
ok('nan', is_nan($cache->fetch('nan')));

$graph = [
    'array' => [-0.0, PHP_FLOAT_MIN, 4.9e-324],
    'object' => (object) ['ratio' => 1 / 3, 'neg' => -0.0],
];
$cache->store('graph', $graph);
$fetched = $cache->fetch('graph');
$arrayOk = true;
foreach ($graph['array'] as $index => $value) {
    $arrayOk = $arrayOk && pack('d', $fetched['array'][$index]) === pack('d', $value);
}
ok('floats in array', $arrayOk);
ok('floats in object', pack('d', $fetched['object']->ratio) === pack('d', 1 / 3)
    && pack('d', $fetched['object']->neg) === pack('d', -0.0));

$cache->store('all', $floats);
ok('serialize parity', serialize($cache->fetch('all')) === serialize($floats));
?>
--EXPECT--
null: OK
bool: OK
int: OK
negative int: OK
float: OK
float INF: OK
float NAN: OK
empty string: OK
binary string: OK
unicode string: OK
empty array: OK
packed array: OK
hashed array: OK
binary keys: OK
deep array: OK
array with references: OK
self-referencing array: OK
stdClass: OK
plain class with visibility: OK
dynamic properties: OK
readonly property: OK
__serialize/__unserialize: OK
__sleep/__wakeup: OK
shared object identity: OK
cyclic object graph: OK
self-referencing object: OK
backed enum case: OK
enum inside array: OK
DateTimeImmutable: OK
DateInterval: OK
ArrayObject: OK
SplStack: OK
SplFixedArray: OK
SplObjectStorage: OK
Randomizer engine object: OK
mixed payload: OK
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
string(7) "alpha:7"
string(5) "alpha"
array(2) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "b"
}
Error
positive zero: OK
negative zero: OK
epsilon: OK
float min: OK
float max: OK
smallest subnormal: OK
one third: OK
point one plus point two: OK
large: OK
small negative: OK
negative zero sign bit: OK
inf: OK
-inf: OK
nan: OK
floats in array: OK
floats in object: OK
serialize parity: OK
