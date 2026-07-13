--TEST--
UserCache\Cache: SplObjectStorage round-trips entries, data, and shared identity
--EXTENSIONS--
user_cache
spl
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
$cache = UserCache\Cache::getPool('spl-object-storage');

function ok(string $label, bool $cond): void
{
	echo $label, ': ', $cond ? 'OK' : 'FAIL', "\n";
}

$a = new stdClass();
$a->id = 1;
$b = new stdClass();
$b->id = 2;
$c = new stdClass();
$c->id = 3;

$storage = new SplObjectStorage();
$storage[$a] = 'scalar-data';
$storage[$b] = ['nested' => [1, 2, 3], 'flag' => true];
$storage[$c] = (object) ['tag' => 'object-data'];

$cache->store('storage', $storage);
$fetched = $cache->fetch('storage');

ok('instanceof', $fetched instanceof SplObjectStorage);
ok('count', count($fetched) === 3);

$byId = [];
foreach ($fetched as $object) {
	$byId[$object->id] = $fetched->getInfo();
}
ok('data preserved', $byId[1] === 'scalar-data'
	&& $byId[2] === ['nested' => [1, 2, 3], 'flag' => true]
	&& $byId[3] instanceof stdClass && $byId[3]->tag === 'object-data');
ok('parity', serialize($fetched) === serialize($storage));

$shared = new stdClass();
$shared->id = 100;
$sharedStorage = new SplObjectStorage();
$sharedStorage[$shared] = 'info';
$graph = ['storage' => $sharedStorage, 'also' => $shared];

$cache->store('graph', $graph);
$fetchedGraph = $cache->fetch('graph');
$fetchedGraph['storage']->rewind();
$keyObject = $fetchedGraph['storage']->current();
ok('shared key identity', $keyObject === $fetchedGraph['also']);
$fetchedGraph['also']->id = 200;
ok('shared mutation follows', $keyObject->id === 200);

$empty = new SplObjectStorage();
$cache->store('empty', $empty);
ok('empty storage', count($cache->fetch('empty')) === 0);

class TaggedStorage extends SplObjectStorage
{
	public string $label = 'default';
}
$tagged = new TaggedStorage();
$tagged->label = 'tagged';
$element = new stdClass();
$element->id = 5;
$tagged[$element] = 'x';
$cache->store('tagged', $tagged);
$fetchedTagged = $cache->fetch('tagged');
ok('subclass instanceof', $fetchedTagged instanceof TaggedStorage);
ok('subclass property', $fetchedTagged->label === 'tagged' && count($fetchedTagged) === 1);
ok('subclass parity', serialize($fetchedTagged) === serialize($tagged));

$inner = new SplObjectStorage();
$innerKey = new stdClass();
$innerKey->id = 7;
$inner[$innerKey] = 'inner';
$outer = new SplObjectStorage();
$outerKey = new stdClass();
$outerKey->id = 8;
$outer[$outerKey] = $inner;
$cache->store('nested', $outer);
$fetchedNested = $cache->fetch('nested');
$fetchedNested->rewind();
$nestedInfo = $fetchedNested->getInfo();
ok('nested storage', $nestedInfo instanceof SplObjectStorage && count($nestedInfo) === 1);
ok('nested parity', serialize($fetchedNested) === serialize($outer));
?>
--EXPECT--
instanceof: OK
count: OK
data preserved: OK
parity: OK
shared key identity: OK
shared mutation follows: OK
empty storage: OK
subclass instanceof: OK
subclass property: OK
subclass parity: OK
nested storage: OK
nested parity: OK
