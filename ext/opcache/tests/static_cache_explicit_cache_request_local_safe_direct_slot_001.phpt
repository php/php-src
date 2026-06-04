--TEST--
OPcache explicit fetch request-local slots clone direct cache objects
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
		var_dump(OPcache\VolatileCache::getInstance('default')->store($key, $value));
	} else {
		OPcache\StableCache::getInstance('default')->store($key, $value);
		echo "stable-stored\n";
	}
}

function fetch_value(string $backend, string $key): mixed
{
	return $backend === 'volatile'
		? OPcache\VolatileCache::getInstance('default')->fetch($key)
		: OPcache\StableCache::getInstance('default')->fetch($key);
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

opcache_static_cache_volatile_reset();
OPcache\StableCache::getInstance('default')->clear();

run_safe_direct_slot_scenario('volatile');
run_safe_direct_slot_scenario('stable');

?>
--EXPECT--
bool(true)
volatile-date-same-object=yes
volatile-date-shared-peer=yes
volatile-date-clone-calls=0
volatile-date-second=2026-01-01:volatile-stored
volatile-date-second-is-first=no
volatile-date-clone-calls=0
bool(true)
volatile-collection-same-object=yes
volatile-collection-shared-peer=yes
volatile-collection-clone-calls=0
volatile-collection-second=volatile-collection-stored:1
volatile-collection-second-is-first=no
volatile-collection-clone-calls=0
stable-stored
stable-date-same-object=yes
stable-date-shared-peer=yes
stable-date-clone-calls=0
stable-date-second=2026-01-01:stable-stored
stable-date-second-is-first=no
stable-date-clone-calls=0
stable-stored
stable-collection-same-object=yes
stable-collection-shared-peer=yes
stable-collection-clone-calls=0
stable-collection-second=stable-collection-stored:1
stable-collection-second-is-first=no
stable-collection-clone-calls=0
