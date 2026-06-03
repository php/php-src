--TEST--
OPcache explicit cache tracks nested userland object mutations across repeated stores
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=32
opcache.static_cache.stable_size_mb=32
--FILE--
<?php

final class PreparedNestedLeaf
{
	public function __construct(
		public string $label,
		public int $revision,
	) {}
}

$payload = [
	'name' => 'first',
	'leaf' => new PreparedNestedLeaf('leaf-first', 1),
	'rows' => [['state' => 'alpha']],
];

var_dump(OPcache\VolatileCache::getInstance('default')->store('nested_volatile_first', $payload));
OPcache\StableCache::getInstance('default')->store('nested_stable_first', $payload);

$payload['name'] = 'second';
$payload['leaf']->label = 'leaf-second';
$payload['leaf']->revision = 2;
$payload['rows'][0]['state'] = 'beta';

var_dump(OPcache\VolatileCache::getInstance('default')->store('nested_volatile_second', $payload));
OPcache\StableCache::getInstance('default')->store('nested_stable_second', $payload);

$volatileFirst = OPcache\VolatileCache::getInstance('default')->fetch('nested_volatile_first');
$volatileSecond = OPcache\VolatileCache::getInstance('default')->fetch('nested_volatile_second');
$stableFirst = OPcache\StableCache::getInstance('default')->fetch('nested_stable_first');
$stableSecond = OPcache\StableCache::getInstance('default')->fetch('nested_stable_second');

echo $volatileFirst['name'], "\n";
echo $volatileFirst['leaf']->label, "\n";
echo $volatileFirst['leaf']->revision, "\n";
echo $volatileFirst['rows'][0]['state'], "\n";
echo $volatileSecond['name'], "\n";
echo $volatileSecond['leaf']->label, "\n";
echo $volatileSecond['leaf']->revision, "\n";
echo $volatileSecond['rows'][0]['state'], "\n";
echo $stableFirst['name'], "\n";
echo $stableFirst['leaf']->label, "\n";
echo $stableFirst['leaf']->revision, "\n";
echo $stableFirst['rows'][0]['state'], "\n";
echo $stableSecond['name'], "\n";
echo $stableSecond['leaf']->label, "\n";
echo $stableSecond['leaf']->revision, "\n";
echo $stableSecond['rows'][0]['state'], "\n";

?>
--EXPECT--
bool(true)
bool(true)
first
leaf-first
1
alpha
second
leaf-second
2
beta
first
leaf-first
1
alpha
second
leaf-second
2
beta
