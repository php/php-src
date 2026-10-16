--TEST--
UserCache\Cache: array keys, next-free index, empty tables and repeated subtrees round-trip exactly
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=16M
--FILE--
<?php
function ok(string $label, bool $cond): void
{
    echo $label, ': ', $cond ? 'OK' : 'FAIL', "\n";
}

/* Negative, sparse, post-unset, list, numeric-string and limit integer keys */
$cache = UserCache\Cache::getPool('arrays-int-keys');

$negative = [-5 => 'a', -1 => 'b', 0 => 'c', -100 => 'd', 7 => 'e'];
$cache->store('negative', $negative);
$fetched = $cache->fetch('negative');
ok('negative keys value', $fetched === $negative);
ok('negative keys order', array_keys($fetched) === [-5, -1, 0, -100, 7]);

$sparse = [0 => 'x', 5 => 'y', 10 => 'z'];
$cache->store('sparse', $sparse);
$fetched = $cache->fetch('sparse');
ok('sparse keys value', $fetched === $sparse);
ok('sparse not list', !array_is_list($fetched));

$packed = [10, 20, 30, 40];
unset($packed[1]);
$cache->store('unset', $packed);
$fetched = $cache->fetch('unset');
ok('post-unset keys', array_keys($fetched) === [0, 2, 3]);
ok('post-unset value', $fetched === $packed);
$fetched[] = 99;
ok('append after unset uses key 4', array_key_last($fetched) === 4);

$list = ['p', 'q', 'r'];
$cache->store('list', $list);
$fetched = $cache->fetch('list');
ok('list is list', array_is_list($fetched));
$fetched[] = 's';
ok('list append index 3', array_key_last($fetched) === 3);

$numeric = ['123' => 'a', '007' => 'b', '-3' => 'c', 'x' => 'd'];
$cache->store('numeric', $numeric);
$fetched = $cache->fetch('numeric');
ok('numeric string key value', $fetched === $numeric);
ok('numeric string key coerced', array_key_first($fetched) === 123 && array_key_exists(-3, $fetched));

$limits = [PHP_INT_MAX => 'max', PHP_INT_MIN => 'min', 0 => 'zero'];
$cache->store('limits', $limits);
$fetched = $cache->fetch('limits');
ok('int limit keys', $fetched[PHP_INT_MAX] === 'max' && $fetched[PHP_INT_MIN] === 'min' && $fetched[0] === 'zero');

$transition = [0 => 'a', 1 => 'b'];
$transition[100] = 'c';
$cache->store('transition', $transition);
$fetched = $cache->fetch('transition');
ok('packed to hashed value', $fetched === $transition);
ok('packed to hashed keys', array_keys($fetched) === [0, 1, 100]);

$nested = [
    'neg' => $negative,
    'sparse' => $sparse,
    'list' => ['deep' => [-2 => 'q', 4 => 'r']],
    9 => [PHP_INT_MAX => 'z'],
];
$cache->store('nested', $nested);
ok('nested mixed keys', $cache->fetch('nested') === $nested);

$all = [$negative, $sparse, $packed, $list, $numeric, $limits, $transition, $nested];
$cache->store('all', $all);
ok('serialize parity', serialize($cache->fetch('all')) === serialize($all));

/* nNextFreeElement survives both the dynamic (object element) and the verbatim (scalar-only) encoding */
$cache = UserCache\Cache::getPool('arrays-next-free');

$negativeObject = [-5 => new stdClass()];
$control = $negativeObject;
$cache->store('negative-object', $negativeObject);
$fetched = $cache->fetch('negative-object');
ok('negative object survived', $fetched[-5] instanceof stdClass);
$control[] = 'y';
$fetched[] = 'y';
ok('negative object control keys', array_keys($control) === [-5, -4]);
ok('negative object fetched keys', array_keys($fetched) === array_keys($control));

$emptied = [];
$emptied[-9] = 1;
unset($emptied[-9]);
$control = $emptied;
$cache->store('emptied-negative', $emptied);
$fetched = $cache->fetch('emptied-negative');
ok('emptied is empty', $fetched === []);
$control[] = 'x';
$fetched[] = 'x';
ok('emptied control keys', array_keys($control) === [-8]);
ok('emptied fetched keys', array_keys($fetched) === array_keys($control));

$hugeObject = [PHP_INT_MAX - 1 => new stdClass()];
$control = $hugeObject;
$cache->store('huge-object', $hugeObject);
$fetched = $cache->fetch('huge-object');
ok('huge object survived', $fetched[PHP_INT_MAX - 1] instanceof stdClass);
$control[] = 'z';
$fetched[] = 'z';
$offsetsFromIntMax = static fn (array $array): string => implode(
    ',',
    array_map(static fn (int $key): int => $key - PHP_INT_MAX, array_keys($array))
);
echo 'huge control keys: ', $offsetsFromIntMax($control), "\n";
echo 'huge fetched keys: ', $offsetsFromIntMax($fetched), "\n";
ok('huge fetched keys match control', array_keys($fetched) === array_keys($control));

$negativePlain = [-5 => 1];
$control = $negativePlain;
$cache->store('negative-plain', $negativePlain);
$fetched = $cache->fetch('negative-plain');
ok('plain value', $fetched === $negativePlain);
$control[] = 'x';
$fetched[] = 'x';
ok('plain control keys', array_keys($control) === [-5, -4]);
ok('plain fetched keys', array_keys($fetched) === array_keys($control));

/* Empty arrays from the lazy (unallocated) table */
$cache = UserCache\Cache::getPool('arrays-empty');

class UserCacheEmptyArrayNode
{
    public ?UserCacheEmptyArrayNode $parent = null;
    public array $children = [];

    public function __construct(public string $name)
    {
    }

    public function __sleep(): array
    {
        return ['parent', 'children', 'name'];
    }

    public function __wakeup(): void
    {
    }
}

var_dump($cache->store('root', []));
var_dump($cache->fetch('root'));

var_dump($cache->store('nested', ['a' => [], 'b' => [[], []], 'c' => ['x' => 1]]));
var_dump($cache->fetch('nested'));

$shared = [];
$wrap = ['first' => &$shared, 'second' => &$shared];
var_dump($cache->store('shared', $wrap));
$fetched = $cache->fetch('shared');
$fetched['first'][] = 'linked';
var_dump($fetched['second']);

$root = new UserCacheEmptyArrayNode('root');
$leaf = new UserCacheEmptyArrayNode('leaf');
$leaf->parent = $root;
$root->children[] = $leaf;
var_dump($cache->store('graph', $root));
$graph = $cache->fetch('graph');
var_dump($graph->children[0]->children);
var_dump($graph->children[0]->parent === $graph);

$empty = $cache->fetch('root');
$empty[] = 42;
var_dump($empty);

/* Release builds do not collect cycles at shutdown. */
unset($shared, $wrap, $fetched, $root, $leaf, $graph, $empty);
gc_collect_cycles();

/* Mixed string/integer key order inside __wakeup objects */
$cache = UserCache\Cache::getPool('arrays-mixed-keys');

class MixedKeyHolder
{
    public array $data = [];

    public function __wakeup(): void {}
}

$cases = [
    'string then int' => ['name' => 'x', 5 => 'y'],
    'strings then int' => ['a' => 1, 'b' => 2, 10 => 3],
    'int then string' => [5 => 'y', 'name' => 'x'],
    'interleaved' => ['a' => 1, 3 => 'x', 'b' => 2, 7 => 'y'],
    'packed' => ['a', 'b'],
    'nested mixed' => ['outer' => [1 => 'one', 'two' => 2], 9 => ['k' => 'v']],
];

foreach ($cases as $label => $data) {
    $holder = new MixedKeyHolder();
    $holder->data = $data;

    var_dump($cache->store($label, $holder));
    $fetched = $cache->fetch($label);
    var_dump($fetched->data === $data);
}

/* Negative next-free index through the __sleep()/__wakeup() path */
$cache = UserCache\Cache::getPool('arrays-serdes-next-free');

class NegativeNextFreeBag
{
    public $arr;

    public function __sleep(): array
    {
        return ['arr'];
    }

    public function __wakeup(): void
    {
    }
}

$bag = new NegativeNextFreeBag();
$bag->arr = [-5 => 'v'];

ok('negative key store', $cache->store('bag', $bag) === true);

$fetched = $cache->fetch('bag', 'default');
ok('negative key fetch decoded', $fetched !== 'default');
ok('negative key class', $fetched instanceof NegativeNextFreeBag);
var_dump($fetched->arr);
$fetched->arr[] = 'appended';
ok('negative key append', array_keys($fetched->arr) === [-5, -4]);
ok('negative key value survived', $fetched->arr[-5] === 'v');

$emptied = [];
$emptied[-9] = 1;
unset($emptied[-9]);

$bag = new NegativeNextFreeBag();
$bag->arr = $emptied;

ok('emptied store', $cache->store('emptied', $bag) === true);

$fetched = $cache->fetch('emptied', 'default');
ok('emptied fetch decoded', $fetched !== 'default');
ok('emptied is empty', $fetched->arr === []);
$fetched->arr[] = 'appended';
ok('emptied append', array_keys($fetched->arr) === [-8]);

/* Repeated arrays in a DAG restore as copy-on-write independent values */
$cache = UserCache\Cache::getPool('arrays-dag');

$shared = [
    'text' => str_repeat('q', 64),
    'list' => range(1, 32),
    'nested' => ['deep' => 'value'],
];

$dag = [
    'first' => $shared,
    'second' => $shared,
    'third' => $shared,
    'wrapped' => [$shared, $shared],
];

var_dump($cache->store('dag', $dag));
$fetched = $cache->fetch('dag');
var_dump($fetched === $dag);

$fetched['first']['nested']['deep'] = 'mutated';
var_dump($fetched['second']['nested']['deep']);
var_dump($fetched['wrapped'][1] === $shared);

$pair = [['k' => 'v'], ['k' => 'v']];
var_dump($cache->store('pair', $pair));
var_dump($cache->fetch('pair') === $pair);

$dag['third']['extra'] = 'tail';
var_dump($cache->store('dag', $dag));
var_dump($cache->fetch('dag') === $dag);
?>
--EXPECT--
negative keys value: OK
negative keys order: OK
sparse keys value: OK
sparse not list: OK
post-unset keys: OK
post-unset value: OK
append after unset uses key 4: OK
list is list: OK
list append index 3: OK
numeric string key value: OK
numeric string key coerced: OK
int limit keys: OK
packed to hashed value: OK
packed to hashed keys: OK
nested mixed keys: OK
serialize parity: OK
negative object survived: OK
negative object control keys: OK
negative object fetched keys: OK
emptied is empty: OK
emptied control keys: OK
emptied fetched keys: OK
huge object survived: OK
huge control keys: -1,0
huge fetched keys: -1,0
huge fetched keys match control: OK
plain value: OK
plain control keys: OK
plain fetched keys: OK
bool(true)
array(0) {
}
bool(true)
array(3) {
  ["a"]=>
  array(0) {
  }
  ["b"]=>
  array(2) {
    [0]=>
    array(0) {
    }
    [1]=>
    array(0) {
    }
  }
  ["c"]=>
  array(1) {
    ["x"]=>
    int(1)
  }
}
bool(true)
array(1) {
  [0]=>
  string(6) "linked"
}
bool(true)
array(0) {
}
bool(true)
array(1) {
  [0]=>
  int(42)
}
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
negative key store: OK
negative key fetch decoded: OK
negative key class: OK
array(1) {
  [-5]=>
  string(1) "v"
}
negative key append: OK
negative key value survived: OK
emptied store: OK
emptied fetch decoded: OK
emptied is empty: OK
emptied append: OK
bool(true)
bool(true)
string(5) "value"
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
