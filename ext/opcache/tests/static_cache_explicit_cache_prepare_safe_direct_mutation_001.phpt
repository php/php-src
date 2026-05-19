--TEST--
OPcache explicit cache does not reuse prepared shared graphs across safe-direct object mutations
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=32
opcache.static_cache.pinned_size_mb=32
--FILE--
<?php

$payload = [
	'name' => 'first',
	'date' => new DateTime('2026-05-01 10:30:45', new DateTimeZone('UTC')),
];

var_dump(OPcache\volatile_store('safe_direct_volatile_first', $payload));
OPcache\pinned_store('safe_direct_pinned_first', $payload);

$payload['name'] = 'second';
$payload['date']->modify('+2 days');

var_dump(OPcache\volatile_store('safe_direct_volatile_second', $payload));
OPcache\pinned_store('safe_direct_pinned_second', $payload);

$volatileFirst = OPcache\volatile_fetch('safe_direct_volatile_first');
$volatileSecond = OPcache\volatile_fetch('safe_direct_volatile_second');
$pinnedFirst = OPcache\pinned_fetch('safe_direct_pinned_first');
$pinnedSecond = OPcache\pinned_fetch('safe_direct_pinned_second');

echo $volatileFirst['name'], "\n";
echo $volatileFirst['date']->format('Y-m-d H:i:s'), "\n";
echo $volatileSecond['name'], "\n";
echo $volatileSecond['date']->format('Y-m-d H:i:s'), "\n";
echo $pinnedFirst['name'], "\n";
echo $pinnedFirst['date']->format('Y-m-d H:i:s'), "\n";
echo $pinnedSecond['name'], "\n";
echo $pinnedSecond['date']->format('Y-m-d H:i:s'), "\n";

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
