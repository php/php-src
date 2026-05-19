--TEST--
OPcache explicit fetch request-local slots clone __DirectCacheSafe objects
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

final class SafeDirectSlotPeer
{
	public function __construct(public string $label) {}
}

class SafeDirectSlotDateTime extends DateTime
{
	public static int $cloneCalls = 0;
	public SafeDirectSlotPeer $peer;

	public function __clone()
	{
		self::$cloneCalls++;
	}
}

class SafeDirectSlotArrayObject extends ArrayObject
{
	public static int $cloneCalls = 0;

	public function __clone()
	{
		self::$cloneCalls++;
	}
}

function store_value(string $backend, string $key, mixed $value): void
{
	if ($backend === 'volatile') {
		var_dump(OPcache\volatile_store($key, $value));
	} else {
		OPcache\pinned_store($key, $value);
		echo "pinned-stored\n";
	}
}

function fetch_value(string $backend, string $key): mixed
{
	return $backend === 'volatile'
		? OPcache\volatile_fetch($key)
		: OPcache\pinned_fetch($key);
}

function run_safe_direct_slot_scenario(string $backend): void
{
	$peer = new SafeDirectSlotPeer($backend . '-stored');
	$date = new SafeDirectSlotDateTime('2026-01-01 00:00:00', new DateTimeZone('UTC'));
	$date->peer = $peer;

	store_value($backend, $backend . '-date', [
		'date' => $date,
		'again' => $date,
		'peer' => $peer,
	]);

	$first = fetch_value($backend, $backend . '-date');
	$warm = fetch_value($backend, $backend . '-date');
	echo $backend, '-date-same-object=', $warm['date'] === $warm['again'] ? 'yes' : 'no', "\n";
	echo $backend, '-date-shared-peer=', $warm['date']->peer === $warm['peer'] ? 'yes' : 'no', "\n";
	echo $backend, '-date-clone-calls=', SafeDirectSlotDateTime::$cloneCalls, "\n";

	$first['date']->modify('+1 day');
	$first['date']->peer->label = $backend . '-changed';
	$second = fetch_value($backend, $backend . '-date');
	echo $backend, '-date-second=', $second['date']->format('Y-m-d'), ':', $second['date']->peer->label, "\n";
	echo $backend, '-date-second-is-first=', $second['date'] === $first['date'] ? 'yes' : 'no', "\n";
	echo $backend, '-date-clone-calls=', SafeDirectSlotDateTime::$cloneCalls, "\n";

	$collectionPeer = new SafeDirectSlotPeer($backend . '-collection-stored');
	$collection = new SafeDirectSlotArrayObject(['peer' => $collectionPeer]);

	store_value($backend, $backend . '-collection', [
		'collection' => $collection,
		'again' => $collection,
		'peer' => $collectionPeer,
	]);

	$collectionFirst = fetch_value($backend, $backend . '-collection');
	$collectionWarm = fetch_value($backend, $backend . '-collection');
	echo $backend, '-collection-same-object=', $collectionWarm['collection'] === $collectionWarm['again'] ? 'yes' : 'no', "\n";
	echo $backend, '-collection-shared-peer=', $collectionWarm['collection']['peer'] === $collectionWarm['peer'] ? 'yes' : 'no', "\n";
	echo $backend, '-collection-clone-calls=', SafeDirectSlotArrayObject::$cloneCalls, "\n";

	$collectionFirst['collection']['peer']->label = $backend . '-collection-changed';
	$collectionFirst['collection']['local'] = 'local-only';
	$collectionSecond = fetch_value($backend, $backend . '-collection');
	echo $backend, '-collection-second=', $collectionSecond['collection']['peer']->label, ':', count($collectionSecond['collection']), "\n";
	echo $backend, '-collection-second-is-first=', $collectionSecond['collection'] === $collectionFirst['collection'] ? 'yes' : 'no', "\n";
	echo $backend, '-collection-clone-calls=', SafeDirectSlotArrayObject::$cloneCalls, "\n";
}

OPcache\volatile_clear();
OPcache\pinned_clear();

run_safe_direct_slot_scenario('volatile');
run_safe_direct_slot_scenario('pinned');

?>
--EXPECT--
bool(true)
volatile-date-same-object=no
volatile-date-shared-peer=no
volatile-date-clone-calls=0
volatile-date-second=2026-01-01:volatile-stored
volatile-date-second-is-first=no
volatile-date-clone-calls=0
bool(true)
volatile-collection-same-object=no
volatile-collection-shared-peer=no
volatile-collection-clone-calls=0
volatile-collection-second=volatile-collection-stored:1
volatile-collection-second-is-first=no
volatile-collection-clone-calls=0
pinned-stored
pinned-date-same-object=no
pinned-date-shared-peer=no
pinned-date-clone-calls=0
pinned-date-second=2026-01-01:pinned-stored
pinned-date-second-is-first=no
pinned-date-clone-calls=0
pinned-stored
pinned-collection-same-object=no
pinned-collection-shared-peer=no
pinned-collection-clone-calls=0
pinned-collection-second=pinned-collection-stored:1
pinned-collection-second-is-first=no
pinned-collection-clone-calls=0
