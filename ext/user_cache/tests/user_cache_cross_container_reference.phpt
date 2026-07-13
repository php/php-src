--TEST--
UserCache\Cache: a single reference shared across array and object containers survives
--EXTENSIONS--
user_cache
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
$cache = UserCache\Cache::getPool('cross-container-reference');

function ok(string $label, bool $cond): void
{
	echo $label, ': ', $cond ? 'OK' : 'FAIL', "\n";
}

class RefHolder
{
	public $slot;
}

$value = 5;
$holder = new RefHolder();
$holder->slot = &$value;
$graph = ['inArray' => &$value, 'holder' => $holder];

$cache->store('scalar', $graph);
$fetched = $cache->fetch('scalar');
ok('initial value', $fetched['inArray'] === 5 && $fetched['holder']->slot === 5);
$fetched['inArray'] = 42;
ok('array write reaches property', $fetched['holder']->slot === 42);
$fetched['holder']->slot = 99;
ok('property write reaches array', $fetched['inArray'] === 99);

$other = $cache->fetch('scalar');
ok('other fetch independent', $other['inArray'] === 5);

class RefPair
{
	public $left;
	public $right;
}

$shared = 1;
$pair = new RefPair();
$pair->left = &$shared;
$pair->right = &$shared;
$wide = ['cell' => &$shared, 'pair' => $pair];
$cache->store('wide', $wide);
$fetchedWide = $cache->fetch('wide');
$fetchedWide['cell'] = 7;
ok('wide alias left', $fetchedWide['pair']->left === 7);
ok('wide alias right', $fetchedWide['pair']->right === 7);

$object = new stdClass();
$object->n = 0;
$objRef = $object;
$objHolder = new RefHolder();
$objHolder->slot = &$objRef;
$objGraph = ['ref' => &$objRef, 'holder' => $objHolder];
$cache->store('object', $objGraph);
$fetchedObj = $cache->fetch('object');
ok('object alias identity', $fetchedObj['ref'] === $fetchedObj['holder']->slot);
$fetchedObj['ref']->n = 5;
ok('object shared mutation', $fetchedObj['holder']->slot->n === 5);

class ArrayPropHolder
{
	public array $data = [];
}
$inner = 3;
$aph = new ArrayPropHolder();
$aph->data['a'] = &$inner;
$aph->data['b'] = &$inner;
$cache->store('array-prop', $aph);
$fetchedAph = $cache->fetch('array-prop');
$fetchedAph->data['a'] = 88;
ok('reference inside object-array property', $fetchedAph->data['b'] === 88);
?>
--EXPECT--
initial value: OK
array write reaches property: OK
property write reaches array: OK
other fetch independent: OK
wide alias left: OK
wide alias right: OK
object alias identity: OK
object shared mutation: OK
reference inside object-array property: OK
