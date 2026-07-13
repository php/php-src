--TEST--
UserCache\Cache: every serializable value class round-trips through store/fetch
--EXTENSIONS--
user_cache
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=32M
--FILE--
<?php
$cache = UserCache\Cache::getPool('coverage');

function check(UserCache\Cache $cache, string $label, mixed $value, ?callable $verify = null): void
{
    static $i = 0;

    $key = 'coverage-' . $i++;
    if (!$cache->store($key, $value)) {
        echo $label, ": STORE FAILED\n";
        return;
    }

    $fetched = $cache->fetch($key);
    if ($verify !== null) {
        $ok = $verify($value, $fetched);
        echo $label, ': ', $ok ? 'OK' : 'MISMATCH', "\n";
        unset($fetched);
        gc_collect_cycles();
        return;
    }

    $ok = serialize($fetched) === serialize($value);
    echo $label, ': ', $ok ? 'OK' : 'MISMATCH', "\n";
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
    /** @var CoverageNode[] */
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

unset($deep, $aliased, $selfref, $dynamic, $shared, $root, $child, $selfobj);
gc_collect_cycles();
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
