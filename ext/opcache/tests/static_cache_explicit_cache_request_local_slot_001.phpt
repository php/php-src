--TEST--
OPcache explicit fetch clones object-bearing request-local results
--EXTENSIONS--
opcache
spl
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=32
opcache.static_cache.stable_size_mb=32
--FILE--
<?php

final class ExplicitRequestLocalSlotPayload
{
	public function __construct(public string $label) {}
}

final class ExplicitRequestLocalSlotUnsupportedList extends SplDoublyLinkedList
{
}

function dump_label(string $prefix, ExplicitRequestLocalSlotPayload $payload): void
{
	echo $prefix, '=', $payload->label, "\n";
}

opcache_static_cache_volatile_reset();
OPcache\StableCache::getInstance('default')->clear();

$volatilePayload = ['object' => new ExplicitRequestLocalSlotPayload('volatile-stored')];
var_dump(OPcache\VolatileCache::getInstance('default')->store('request_local_slot', $volatilePayload));

$volatileFirst = OPcache\VolatileCache::getInstance('default')->fetch('request_local_slot');
$volatileWarm = OPcache\VolatileCache::getInstance('default')->fetch('request_local_slot');
var_dump($volatileFirst['object'] === $volatileWarm['object']);
$volatileFirst['object']->label = 'volatile-mutated-in-request';
$volatileSecond = OPcache\VolatileCache::getInstance('default')->fetch('request_local_slot');

var_dump($volatileFirst['object'] === $volatileSecond['object']);
dump_label('volatile-second', $volatileSecond['object']);

var_dump(OPcache\VolatileCache::getInstance('default')->store('request_local_slot', ['object' => new ExplicitRequestLocalSlotPayload('volatile-replaced')]));
$volatileReplaced = OPcache\VolatileCache::getInstance('default')->fetch('request_local_slot');
dump_label('volatile-replaced', $volatileReplaced['object']);

OPcache\VolatileCache::getInstance('default')->delete('request_local_slot');
var_dump(OPcache\VolatileCache::getInstance('default')->fetch('request_local_slot', 'volatile-missing'));

var_dump(OPcache\StableCache::getInstance('default')->storeWithTtl('request_local_ttl', ['object' => new ExplicitRequestLocalSlotPayload('stable-ttl')], 1));
$stableTtl = OPcache\StableCache::getInstance('default')->fetch('request_local_ttl');
dump_label('stable-ttl-first', $stableTtl['object']);
sleep(2);
var_dump(OPcache\StableCache::getInstance('default')->fetch('request_local_ttl', 'stable-expired'));

var_dump(OPcache\VolatileCache::getInstance('default')->store('request_local_datetime', new DateTime('2026-01-01 00:00:00', new DateTimeZone('UTC'))));
$volatileDateFirst = OPcache\VolatileCache::getInstance('default')->fetch('request_local_datetime');
$volatileDateWarm = OPcache\VolatileCache::getInstance('default')->fetch('request_local_datetime');
var_dump($volatileDateFirst === $volatileDateWarm);
$volatileDateFirst->modify('+1 day');
$volatileDateSecond = OPcache\VolatileCache::getInstance('default')->fetch('request_local_datetime');
var_dump($volatileDateFirst === $volatileDateSecond);
echo $volatileDateSecond->format('Y-m-d'), "\n";

var_dump(OPcache\VolatileCache::getInstance('default')->store('request_local_array_object', new ArrayObject(['label' => 'stored'])));
$volatileArrayObjectFirst = OPcache\VolatileCache::getInstance('default')->fetch('request_local_array_object');
$volatileArrayObjectWarm = OPcache\VolatileCache::getInstance('default')->fetch('request_local_array_object');
var_dump($volatileArrayObjectFirst === $volatileArrayObjectWarm);
$volatileArrayObjectFirst->exchangeArray(['label' => 'changed']);
$volatileArrayObjectSecond = OPcache\VolatileCache::getInstance('default')->fetch('request_local_array_object');
var_dump($volatileArrayObjectFirst === $volatileArrayObjectSecond);
echo $volatileArrayObjectSecond['label'], "\n";

$volatileList = new ExplicitRequestLocalSlotUnsupportedList();
$volatileList->push('stored');
var_dump(OPcache\VolatileCache::getInstance('default')->store('request_local_unsupported_internal', $volatileList));
$volatileListFirst = OPcache\VolatileCache::getInstance('default')->fetch('request_local_unsupported_internal');
$volatileListWarm = OPcache\VolatileCache::getInstance('default')->fetch('request_local_unsupported_internal');
var_dump($volatileListFirst === $volatileListWarm);
$volatileListFirst->push('changed');
$volatileListSecond = OPcache\VolatileCache::getInstance('default')->fetch('request_local_unsupported_internal');
var_dump($volatileListFirst === $volatileListSecond);
echo $volatileListSecond->count(), ':', $volatileListSecond->bottom(), "\n";

OPcache\StableCache::getInstance('default')->store('request_local_slot', ['object' => new ExplicitRequestLocalSlotPayload('stable-stored')]);
$stableFirst = OPcache\StableCache::getInstance('default')->fetch('request_local_slot');
$stableWarm = OPcache\StableCache::getInstance('default')->fetch('request_local_slot');
var_dump($stableFirst['object'] === $stableWarm['object']);
$stableFirst['object']->label = 'stable-mutated-in-request';
$stableSecond = OPcache\StableCache::getInstance('default')->fetch('request_local_slot');

var_dump($stableFirst['object'] === $stableSecond['object']);
dump_label('stable-second', $stableSecond['object']);

OPcache\StableCache::getInstance('default')->store('request_local_slot', ['object' => new ExplicitRequestLocalSlotPayload('stable-replaced')]);
$stableReplaced = OPcache\StableCache::getInstance('default')->fetch('request_local_slot');
dump_label('stable-replaced', $stableReplaced['object']);

OPcache\StableCache::getInstance('default')->delete('request_local_slot');
var_dump(OPcache\StableCache::getInstance('default')->fetch('request_local_slot', 'stable-missing'));

?>
--EXPECT--
bool(true)
bool(false)
bool(false)
volatile-second=volatile-stored
bool(true)
volatile-replaced=volatile-replaced
string(16) "volatile-missing"
bool(true)
stable-ttl-first=stable-ttl
string(14) "stable-expired"
bool(true)
bool(false)
bool(false)
2026-01-01
bool(true)
bool(false)
bool(false)
stored
bool(true)
bool(false)
bool(false)
1:stored
bool(false)
bool(false)
stable-second=stable-stored
stable-replaced=stable-replaced
string(14) "stable-missing"
