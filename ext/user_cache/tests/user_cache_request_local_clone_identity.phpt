--TEST--
UserCache\Cache: request-local clones preserve shared identity and stay independent across fetches
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
$cache = UserCache\Cache::getPool('request-local-clone-identity');
$cache->clear();

class UCCloneLeaf
{
	public function __construct(public int $n)
	{
	}
}

#[AllowDynamicProperties]
class UCCloneNode
{
	public UCCloneLeaf $typed;
	public $shared;
	public $again;
	public array $list = [];
	public ?UCCloneNode $self = null;
	public int $typedRef = 0;
}

class UCClonePlain
{
	public function __construct(
		public array $items,
		public UCCloneLeaf $leaf,
		public ArrayObject $bag,
	) {
	}
}

function uc_check_aliased(array $v, string $label): void
{
	$node = $v['node'];
	$leaf = $v['leaf'];

	$ok = $node instanceof UCCloneNode
		&& $leaf instanceof UCCloneLeaf
		&& $leaf->n === 1
		&& $node->typed === $leaf
		&& $node->shared === $leaf
		&& $node->again === $leaf
		&& $node->dynamic === $leaf
		&& $node->self === $node
		&& $node->list[0] === $leaf
		&& $node->list[2] instanceof ArrayObject
		&& $node->list[2][0] === $leaf
		&& $node->list[2][1] === $leaf
		&& $node->list[3] instanceof SplFixedArray
		&& $node->list[3][0] === $leaf
		&& $node->list[3][1] === 7
		&& $v['queue'] instanceof SplQueue
		&& $v['queue'][0] === $leaf
		&& $v['queue'][1] === $leaf
		&& $v['ref1'] === ['x', 'y']
		&& $v['ref2'] === ['x', 'y'];

	/* One shared reference: writing through any alias is visible through the others. */
	$v['ref1'][] = 'z';
	$ok = $ok
		&& $v['ref2'] === ['x', 'y', 'z']
		&& $node->list[1] === ['x', 'y', 'z'];

	/* A shared array keeps one identity, so the object inside it is one object. */
	$ok = $ok
		&& $v['arr1'][0] === $v['arr2'][0]
		&& $v['arr1'][0]->n === 8
		&& $v['arr1'][1] === 'tail';

	/* The typed property still shares the reference, and the clone keeps its type source. */
	$v['counter'] = 5;
	$ok = $ok && $node->typedRef === 5;
	try {
		$v['counter'] = 'not-an-int';
		$ok = false;
	} catch (TypeError $e) {
	}

	echo $label, ': ', $ok ? 'ok' : 'FAIL', "\n";
}

function uc_check_plain(UCClonePlain $p, string $label): void
{
	$ok = $p->items === [1, [2, 3], 'k' => 'v']
		&& $p->leaf instanceof UCCloneLeaf
		&& $p->leaf->n === 5
		&& $p->bag instanceof ArrayObject
		&& count($p->bag) === 1
		&& $p->bag[0] instanceof UCCloneLeaf
		&& $p->bag[0]->n === 6
		&& $p->bag[0] !== $p->leaf;

	echo $label, ': ', $ok ? 'ok' : 'FAIL', "\n";
}

$leaf = new UCCloneLeaf(1);
$arr = ['x', 'y'];
$node = new UCCloneNode();
$node->typed = $leaf;
$node->shared = $leaf;
$node->again = $leaf;
$node->dynamic = $leaf;
$node->list = [$leaf, &$arr, new ArrayObject([$leaf, $leaf]), SplFixedArray::fromArray([$leaf, 7])];
$node->self = $node;
$queue = new SplQueue();
$queue->push($leaf);
$queue->push($leaf);
$sharedArr = [new UCCloneLeaf(8), 'tail'];
$counter = 0;
$node->typedRef = &$counter;

$aliased = [
	'node' => $node,
	'leaf' => $leaf,
	'ref1' => &$arr,
	'ref2' => &$arr,
	'queue' => $queue,
	'arr1' => $sharedArr,
	'arr2' => $sharedArr,
	'counter' => &$counter,
];

var_dump($cache->store('aliased', $aliased));

$first = $cache->fetch('aliased');
uc_check_aliased($first, 'aliased fetch 1');
$second = $cache->fetch('aliased');
uc_check_aliased($second, 'aliased fetch 2');
$third = $cache->fetch('aliased');
uc_check_aliased($third, 'aliased fetch 3');

/* Every fetch is an independent clone of the cached graph. */
$first['leaf']->n = 42;
$first['node']->list[2][0]->n = 43;
echo 'aliased independent: ';
var_dump($first['leaf']->n === 43
	&& $second['leaf']->n === 1
	&& $third['leaf']->n === 1
	&& $first['node'] !== $second['node']
	&& $first['leaf'] !== $second['leaf']);

var_dump($cache->store('plain', new UCClonePlain([1, [2, 3], 'k' => 'v'], new UCCloneLeaf(5), new ArrayObject([new UCCloneLeaf(6)]))));

$p1 = $cache->fetch('plain');
uc_check_plain($p1, 'plain fetch 1');
$p2 = $cache->fetch('plain');
uc_check_plain($p2, 'plain fetch 2');
$p3 = $cache->fetch('plain');
uc_check_plain($p3, 'plain fetch 3');

$p1->leaf->n = 9;
$p1->bag[0]->n = 10;
echo 'plain independent: ';
var_dump($p1->leaf->n === 9 && $p2->leaf->n === 5 && $p3->leaf->n === 5 && $p2->bag[0]->n === 6 && $p1 !== $p2);
?>
--EXPECT--
bool(true)
aliased fetch 1: ok
aliased fetch 2: ok
aliased fetch 3: ok
aliased independent: bool(true)
bool(true)
plain fetch 1: ok
plain fetch 2: ok
plain fetch 3: ok
plain independent: bool(true)
