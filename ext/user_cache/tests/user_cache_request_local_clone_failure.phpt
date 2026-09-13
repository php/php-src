--TEST--
UserCache\Cache: request-local clone failures fall back to a full decode without leaking
--SKIPIF--
<?php
if (!PHP_DEBUG) die('skip requires a debug build (fault injection is ZEND_DEBUG-only)');
?>
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
$cache = UserCache\Cache::getPool('request-local-clone-failure');
$cache->clear();

class UCCloneOkLeaf
{
	public function __construct(public int $n)
	{
	}
}

class UCCloneFailLeaf
{
	public function __construct(public int $n)
	{
	}
}

class UCCloneHolder
{
	public $first;
	public $second;
	public array $list = [];
}

class UCCloneBag extends ArrayObject
{
	public $extra;
}

function uc_leaf_values(iterable $items): array
{
	$out = [];
	foreach ($items as $item) {
		$out[] = $item instanceof UCCloneOkLeaf || $item instanceof UCCloneFailLeaf ? $item->n : $item;
	}

	return $out;
}

function uc_check(string $key, mixed $v): bool
{
	switch ($key) {
		case 'array':
			return uc_leaf_values($v) === [1, 2, 1] && $v[0] === $v[2];
		case 'array-untracked':
			return uc_leaf_values($v) === [3, 4];
		case 'reference':
			$ok = $v['r1'] === $v['r2'] && $v['r1'] instanceof UCCloneFailLeaf && $v['r1']->n === 5 && $v['shared'] === $v['again'];
			$v['r1'] = 'replaced';

			return $ok && $v['r2'] === 'replaced';
		case 'object':
			return $v instanceof UCCloneHolder
				&& $v->first === $v->list[0]
				&& $v->first->n === 1
				&& $v->second instanceof UCCloneFailLeaf
				&& $v->second->n === 2;
		case 'object-untracked':
			return $v instanceof UCCloneHolder && $v->first->n === 6 && $v->second->n === 7;
		case 'arrayobject':
			return $v instanceof ArrayObject
				&& uc_leaf_values($v) === [1, 2]
				&& $v[0] instanceof UCCloneOkLeaf
				&& $v[1] instanceof UCCloneFailLeaf;
		case 'arrayobject-member':
			return $v instanceof UCCloneBag
				&& uc_leaf_values($v) === [1, 1]
				&& $v[0] instanceof UCCloneOkLeaf
				&& $v[0] === $v[1]
				&& $v->extra instanceof UCCloneFailLeaf
				&& $v->extra->n === 2;
		case 'arrayobject-member-untracked':
			return $v instanceof UCCloneBag
				&& uc_leaf_values($v) === [8]
				&& $v->extra instanceof UCCloneFailLeaf
				&& $v->extra->n === 9;
		case 'fixedarray':
			return $v instanceof SplFixedArray && uc_leaf_values($v) === [1, 2, 1] && $v[0] === $v[2];
		case 'queue':
			return $v instanceof SplQueue && uc_leaf_values($v) === [1, 2, 1] && $v[0] === $v[2];
	}

	return false;
}

$ok = new UCCloneOkLeaf(1);
$fail = new UCCloneFailLeaf(2);
$failRef = new UCCloneFailLeaf(5);

$holder = new UCCloneHolder();
$holder->first = $ok;
$holder->second = $fail;
$holder->list = [$ok];

$untrackedHolder = new UCCloneHolder();
$untrackedHolder->first = new UCCloneOkLeaf(6);
$untrackedHolder->second = new UCCloneFailLeaf(7);

$queue = new SplQueue();
$queue->push($ok);
$queue->push($fail);
$queue->push($ok);

/* The container state clones fine; the failing object is a declared member. */
$aoMember = new UCCloneBag([$ok, $ok]);
$aoMember->extra = $fail;
$aoMemberUntracked = new UCCloneBag([new UCCloneOkLeaf(8)]);
$aoMemberUntracked->extra = new UCCloneFailLeaf(9);

$graphs = [
	'array' => [$ok, $fail, $ok],
	'array-untracked' => [new UCCloneOkLeaf(3), new UCCloneFailLeaf(4)],
	'reference' => ['r1' => &$failRef, 'r2' => &$failRef, 'shared' => $ok, 'again' => $ok],
	'object' => $holder,
	'object-untracked' => $untrackedHolder,
	'arrayobject' => new ArrayObject([$ok, $fail]),
	'arrayobject-member' => $aoMember,
	'arrayobject-member-untracked' => $aoMemberUntracked,
	'fixedarray' => SplFixedArray::fromArray([$ok, $fail, $ok]),
	'queue' => $queue,
];

foreach ($graphs as $key => $graph) {
	var_dump($cache->store($key, $graph));
}

/* Round 1: no fault injected, so the first fetch seeds each request-local slot. */
putenv('USER_CACHE_DEBUG_FAIL_REQUEST_LOCAL_CLONE_CLASS');
foreach ($graphs as $key => $graph) {
	echo 'seed ', $key, ': ', uc_check($key, $cache->fetch($key)) ? 'ok' : 'FAIL', "\n";
}

/* Round 2: cloning any UCCloneFailLeaf fails, so slot materialization and
 * re-seeding both fail part-way and every fetch falls back to a full decode. */
putenv('USER_CACHE_DEBUG_FAIL_REQUEST_LOCAL_CLONE_CLASS=UCCloneFailLeaf');
foreach ($graphs as $key => $graph) {
	$a = $cache->fetch($key);
	$b = $cache->fetch($key);
	echo 'fallback ', $key, ': ', uc_check($key, $a) && uc_check($key, $b) ? 'ok' : 'FAIL', "\n";
}

/* Round 3: fault cleared, slots seed again. */
putenv('USER_CACHE_DEBUG_FAIL_REQUEST_LOCAL_CLONE_CLASS');
foreach ($graphs as $key => $graph) {
	$a = $cache->fetch($key);
	$b = $cache->fetch($key);
	echo 'recover ', $key, ': ', uc_check($key, $a) && uc_check($key, $b) ? 'ok' : 'FAIL', "\n";
}
?>
--EXPECT--
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
seed array: ok
seed array-untracked: ok
seed reference: ok
seed object: ok
seed object-untracked: ok
seed arrayobject: ok
seed arrayobject-member: ok
seed arrayobject-member-untracked: ok
seed fixedarray: ok
seed queue: ok
fallback array: ok
fallback array-untracked: ok
fallback reference: ok
fallback object: ok
fallback object-untracked: ok
fallback arrayobject: ok
fallback arrayobject-member: ok
fallback arrayobject-member-untracked: ok
fallback fixedarray: ok
fallback queue: ok
recover array: ok
recover array-untracked: ok
recover reference: ok
recover object: ok
recover object-untracked: ok
recover arrayobject: ok
recover arrayobject-member: ok
recover arrayobject-member-untracked: ok
recover fixedarray: ok
recover queue: ok
