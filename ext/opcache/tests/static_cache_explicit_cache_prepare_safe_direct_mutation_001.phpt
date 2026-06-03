--TEST--
OPcache explicit cache does not reuse prepared shared graphs across safe-direct object mutations
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=32
opcache.static_cache.stable_size_mb=32
--FILE--
<?php

$payload = [
	'name' => 'first',
	'date' => new DateTime('2026-05-01 10:30:45', new DateTimeZone('UTC')),
];

var_dump(OPcache\VolatileCache::getInstance('default')->store('safe_direct_volatile_first', $payload));
OPcache\StableCache::getInstance('default')->store('safe_direct_stable_first', $payload);

$payload['name'] = 'second';
$payload['date']->modify('+2 days');

var_dump(OPcache\VolatileCache::getInstance('default')->store('safe_direct_volatile_second', $payload));
OPcache\StableCache::getInstance('default')->store('safe_direct_stable_second', $payload);

$volatileFirst = OPcache\VolatileCache::getInstance('default')->fetch('safe_direct_volatile_first');
$volatileSecond = OPcache\VolatileCache::getInstance('default')->fetch('safe_direct_volatile_second');
$stableFirst = OPcache\StableCache::getInstance('default')->fetch('safe_direct_stable_first');
$stableSecond = OPcache\StableCache::getInstance('default')->fetch('safe_direct_stable_second');

echo $volatileFirst['name'], "\n";
echo $volatileFirst['date']->format('Y-m-d H:i:s'), "\n";
echo $volatileSecond['name'], "\n";
echo $volatileSecond['date']->format('Y-m-d H:i:s'), "\n";
echo $stableFirst['name'], "\n";
echo $stableFirst['date']->format('Y-m-d H:i:s'), "\n";
echo $stableSecond['name'], "\n";
echo $stableSecond['date']->format('Y-m-d H:i:s'), "\n";

?>
--EXPECT--
bool(true)
bool(true)
first
2026-05-01 10:30:45
second
2026-05-03 10:30:45
first
2026-05-01 10:30:45
second
2026-05-03 10:30:45
