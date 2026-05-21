--TEST--
OPcache explicit fetch clones object-bearing request-local results
--EXTENSIONS--
opcache
spl
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=32
opcache.static_cache.pinned_size_mb=32
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

OPcache\volatile_clear();
OPcache\pinned_clear();

$volatilePayload = ['object' => new ExplicitRequestLocalSlotPayload('volatile-stored')];
var_dump(OPcache\volatile_store('request_local_slot', $volatilePayload));

$volatileFirst = OPcache\volatile_fetch('request_local_slot');
$volatileWarm = OPcache\volatile_fetch('request_local_slot');
var_dump($volatileFirst['object'] === $volatileWarm['object']);
$volatileFirst['object']->label = 'volatile-mutated-in-request';
$volatileSecond = OPcache\volatile_fetch('request_local_slot');

var_dump($volatileFirst['object'] === $volatileSecond['object']);
dump_label('volatile-second', $volatileSecond['object']);

var_dump(OPcache\volatile_store('request_local_slot', ['object' => new ExplicitRequestLocalSlotPayload('volatile-replaced')]));
$volatileReplaced = OPcache\volatile_fetch('request_local_slot');
dump_label('volatile-replaced', $volatileReplaced['object']);

OPcache\volatile_delete('request_local_slot');
var_dump(OPcache\volatile_fetch('request_local_slot', 'volatile-missing'));

var_dump(OPcache\volatile_store('request_local_ttl', ['object' => new ExplicitRequestLocalSlotPayload('volatile-ttl')], 1));
$volatileTtl = OPcache\volatile_fetch('request_local_ttl');
dump_label('volatile-ttl-first', $volatileTtl['object']);
sleep(2);
var_dump(OPcache\volatile_fetch('request_local_ttl', 'volatile-expired'));

var_dump(OPcache\volatile_store('request_local_datetime', new DateTime('2026-01-01 00:00:00', new DateTimeZone('UTC'))));
$volatileDateFirst = OPcache\volatile_fetch('request_local_datetime');
$volatileDateWarm = OPcache\volatile_fetch('request_local_datetime');
var_dump($volatileDateFirst === $volatileDateWarm);
$volatileDateFirst->modify('+1 day');
$volatileDateSecond = OPcache\volatile_fetch('request_local_datetime');
var_dump($volatileDateFirst === $volatileDateSecond);
echo $volatileDateSecond->format('Y-m-d'), "\n";

var_dump(OPcache\volatile_store('request_local_array_object', new ArrayObject(['label' => 'stored'])));
$volatileArrayObjectFirst = OPcache\volatile_fetch('request_local_array_object');
$volatileArrayObjectWarm = OPcache\volatile_fetch('request_local_array_object');
var_dump($volatileArrayObjectFirst === $volatileArrayObjectWarm);
$volatileArrayObjectFirst->exchangeArray(['label' => 'changed']);
$volatileArrayObjectSecond = OPcache\volatile_fetch('request_local_array_object');
var_dump($volatileArrayObjectFirst === $volatileArrayObjectSecond);
echo $volatileArrayObjectSecond['label'], "\n";

$volatileList = new ExplicitRequestLocalSlotUnsupportedList();
$volatileList->push('stored');
var_dump(OPcache\volatile_store('request_local_unsupported_internal', $volatileList));
$volatileListFirst = OPcache\volatile_fetch('request_local_unsupported_internal');
$volatileListWarm = OPcache\volatile_fetch('request_local_unsupported_internal');
var_dump($volatileListFirst === $volatileListWarm);
$volatileListFirst->push('changed');
$volatileListSecond = OPcache\volatile_fetch('request_local_unsupported_internal');
var_dump($volatileListFirst === $volatileListSecond);
echo $volatileListSecond->count(), ':', $volatileListSecond->bottom(), "\n";

OPcache\pinned_store('request_local_slot', ['object' => new ExplicitRequestLocalSlotPayload('pinned-stored')]);
$pinnedFirst = OPcache\pinned_fetch('request_local_slot');
$pinnedWarm = OPcache\pinned_fetch('request_local_slot');
var_dump($pinnedFirst['object'] === $pinnedWarm['object']);
$pinnedFirst['object']->label = 'pinned-mutated-in-request';
$pinnedSecond = OPcache\pinned_fetch('request_local_slot');

var_dump($pinnedFirst['object'] === $pinnedSecond['object']);
dump_label('pinned-second', $pinnedSecond['object']);

OPcache\pinned_store('request_local_slot', ['object' => new ExplicitRequestLocalSlotPayload('pinned-replaced')]);
$pinnedReplaced = OPcache\pinned_fetch('request_local_slot');
dump_label('pinned-replaced', $pinnedReplaced['object']);

OPcache\pinned_delete('request_local_slot');
var_dump(OPcache\pinned_fetch('request_local_slot', 'pinned-missing'));

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
volatile-ttl-first=volatile-ttl
string(16) "volatile-expired"
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
pinned-second=pinned-stored
pinned-replaced=pinned-replaced
string(14) "pinned-missing"
