--TEST--
UserCache\Cache: uninitialized typed properties round-trip as uninitialized
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
$cache = UserCache\Cache::getPool('uninitialized-typed-prop');

function ok(string $label, bool $cond): void
{
	echo $label, ': ', $cond ? 'OK' : 'FAIL', "\n";
}

class UninitBox
{
	public int $assigned = 0;
	public int $neverAssigned;
	public ?string $nullableNeverAssigned;
	public string $withDefault = 'default';
}

$object = new UninitBox();
$object->assigned = 42;

$cache->store('box', $object);
$fetched = $cache->fetch('box');

$assignedProp = new ReflectionProperty($fetched, 'assigned');
$neverProp = new ReflectionProperty($fetched, 'neverAssigned');
$nullableProp = new ReflectionProperty($fetched, 'nullableNeverAssigned');
$defaultProp = new ReflectionProperty($fetched, 'withDefault');

ok('assigned initialized', $assignedProp->isInitialized($fetched) && $fetched->assigned === 42);
ok('never-assigned stays uninitialized', !$neverProp->isInitialized($fetched));
ok('nullable-never stays uninitialized', !$nullableProp->isInitialized($fetched));
ok('default initialized', $defaultProp->isInitialized($fetched) && $fetched->withDefault === 'default');

$readError = false;
try {
	$fetched->neverAssigned;
} catch (Error $e) {
	$readError = str_contains($e->getMessage(), 'must not be accessed before initialization');
}
ok('uninitialized read still errors', $readError);

$fetched->neverAssigned = 7;
ok('can assign after fetch', $fetched->neverAssigned === 7);
$typeError = false;
try {
	$fetched->assigned = 'not an int';
} catch (TypeError $e) {
	$typeError = true;
}
ok('type check preserved', $typeError);

class UninitReadonly
{
	public readonly int $id;
	public string $name;
	public function __construct(string $name)
	{
		$this->name = $name;
	}
}
$ro = new UninitReadonly('n');
$cache->store('ro', $ro);
$fetchedRo = $cache->fetch('ro');
$idProp = new ReflectionProperty($fetchedRo, 'id');
ok('readonly uninitialized survives', !$idProp->isInitialized($fetchedRo) && $fetchedRo->name === 'n');

$graph = ['direct' => new UninitBox(), 'list' => [new UninitBox()]];
$graph['direct']->assigned = 1;
$cache->store('graph', $graph);
$fetchedGraph = $cache->fetch('graph');
$nestedProp = new ReflectionProperty($fetchedGraph['list'][0], 'neverAssigned');
ok('nested uninitialized survives', !$nestedProp->isInitialized($fetchedGraph['list'][0]));
ok('serialize parity', serialize($cache->fetch('box')) === serialize($object));
?>
--EXPECT--
assigned initialized: OK
never-assigned stays uninitialized: OK
nullable-never stays uninitialized: OK
default initialized: OK
uninitialized read still errors: OK
can assign after fetch: OK
type check preserved: OK
readonly uninitialized survives: OK
nested uninitialized survives: OK
serialize parity: OK
