--TEST--
OPcache volatile cache safe-direct restores SPL array object state and properties
--EXTENSIONS--
opcache
spl
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=32
--FILE--
<?php

class StaticCacheSplArrayIterator extends ArrayIterator
{
}

final class StaticCacheSplArrayPayload extends ArrayObject
{
	public function __construct(
		array $storage,
		public string $name,
		public int $revision,
	) {
		parent::__construct($storage, ArrayObject::ARRAY_AS_PROPS, StaticCacheSplArrayIterator::class);
	}
}

opcache_static_cache_volatile_reset();

$rows = [
	'alpha' => ['score' => 11],
	'beta' => ['score' => 17],
];
$payload = new StaticCacheSplArrayPayload([
	'map' => new ArrayObject($rows, ArrayObject::ARRAY_AS_PROPS),
	'iterator' => new ArrayIterator($rows),
	'recursive' => new RecursiveArrayIterator([
		'branch' => [
			'leaf' => ['score' => 23],
		],
	]),
], 'spl-array', 5);

var_dump(OPcache\VolatileCache::getInstance('default')->store('spl-array-direct', $payload));

$first = OPcache\VolatileCache::getInstance('default')->fetch('spl-array-direct');
$first['map']['alpha']['score'] = 99;
$first->name = 'changed';

$second = OPcache\VolatileCache::getInstance('default')->fetch('spl-array-direct');

var_dump($second instanceof StaticCacheSplArrayPayload);
var_dump($second->name);
var_dump($second->revision);
var_dump($second->map instanceof ArrayObject);
var_dump($second['map'] instanceof ArrayObject);
var_dump($second['map']['alpha']['score']);
var_dump($second['iterator'] instanceof ArrayIterator);
var_dump($second['iterator']['beta']['score']);
var_dump($second['recursive'] instanceof RecursiveArrayIterator);
var_dump($second['recursive']['branch']['leaf']['score']);
$iterator = $second->getIterator();
var_dump($iterator instanceof StaticCacheSplArrayIterator);

?>
--EXPECT--
bool(true)
bool(true)
string(9) "spl-array"
int(5)
bool(true)
bool(true)
int(11)
bool(true)
int(17)
bool(true)
int(23)
bool(true)
