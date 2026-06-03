--TEST--
OPcache direct cache handlers use a direct DateTime path for safe subclasses and keep fallback for wakeup hooks
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=32
--FILE--
<?php

$immutable = new DateTimeImmutable('2026-06-15 11:00:00.111111', new DateTimeZone('UTC'));

var_dump(OPcache\VolatileCache::getInstance('default')->store('immutable_datetime', $immutable));

$immutableCopy = OPcache\VolatileCache::getInstance('default')->fetch('immutable_datetime');
var_dump($immutableCopy instanceof DateTimeImmutable);
var_dump($immutableCopy->format('Y-m-d H:i:s.u e'));

$offsetImmutable = new DateTimeImmutable('2023-10-27 10:00:00 +05:30');

var_dump(OPcache\VolatileCache::getInstance('default')->store('offset_immutable_datetime', $offsetImmutable));

$offsetImmutableCopy = OPcache\VolatileCache::getInstance('default')->fetch('offset_immutable_datetime');
var_dump($offsetImmutableCopy instanceof DateTimeImmutable);
var_dump($offsetImmutableCopy->format('Y-m-d H:i:s P T'));
var_dump($offsetImmutableCopy->getTimezone()->getName());

$abbrImmutable = new DateTimeImmutable('2023-10-27 10:00:00 EST');

var_dump(OPcache\VolatileCache::getInstance('default')->store('abbr_immutable_datetime', $abbrImmutable));

$abbrImmutableCopy = OPcache\VolatileCache::getInstance('default')->fetch('abbr_immutable_datetime');
var_dump($abbrImmutableCopy instanceof DateTimeImmutable);
var_dump($abbrImmutableCopy->format('Y-m-d H:i:s P T'));
var_dump($abbrImmutableCopy->getTimezone()->getName());

class EventDateTime extends DateTime
{
	private string $label;
	protected int $revision;

	public function __construct(string $time, DateTimeZone $timezone, string $label, int $revision)
	{
		parent::__construct($time, $timezone);
		$this->label = $label;
		$this->revision = $revision;
	}

	public function describe(): string
	{
		return $this->label . ':' . $this->revision;
	}
}

$event = new EventDateTime('2026-06-15 09:30:00.123456', new DateTimeZone('Europe/Paris'), 'launch', 7);

var_dump(OPcache\VolatileCache::getInstance('default')->store('event_datetime', $event));

$eventCopy = OPcache\VolatileCache::getInstance('default')->fetch('event_datetime');
var_dump($eventCopy instanceof EventDateTime);
var_dump($eventCopy->format('Y-m-d H:i:s.u e'));
var_dump($eventCopy->describe());

class CustomSerializedDateTime extends DateTime
{
	public static int $serializeCount = 0;
	public static int $unserializeCount = 0;

	private string $label;

	public function __construct(string $time, DateTimeZone $timezone, string $label)
	{
		parent::__construct($time, $timezone);
		$this->label = $label;
	}

	public function __serialize(): array
	{
		self::$serializeCount++;

		return parent::__serialize() + ['label' => $this->label];
	}

	public function __unserialize(array $data): void
	{
		self::$unserializeCount++;
		parent::__unserialize($data);
		$this->label = $data['label'];
	}

	public function label(): string
	{
		return $this->label;
	}
}

$custom = new CustomSerializedDateTime('2026-06-15 10:45:00.654321', new DateTimeZone('UTC'), 'fallback');

var_dump(OPcache\VolatileCache::getInstance('default')->store('custom_datetime', $custom));

$customCopy = OPcache\VolatileCache::getInstance('default')->fetch('custom_datetime');
var_dump($customCopy instanceof CustomSerializedDateTime);
var_dump($customCopy->format('Y-m-d H:i:s.u e'));
var_dump($customCopy->label());
var_dump(CustomSerializedDateTime::$serializeCount);
var_dump(CustomSerializedDateTime::$unserializeCount);

class WakefulSerializedDateTime extends DateTime
{
	public static int $serializeCount = 0;
	public static int $unserializeCount = 0;

	private string $label;

	public function __construct(string $time, DateTimeZone $timezone, string $label)
	{
		parent::__construct($time, $timezone);
		$this->label = $label;
	}

	public function __serialize(): array
	{
		self::$serializeCount++;

		return parent::__serialize() + ['label' => $this->label];
	}

	public function __unserialize(array $data): void
	{
		self::$unserializeCount++;
		parent::__unserialize($data);
		$this->label = $data['label'];
	}

	public function __sleep(): array
	{
		return ['label'];
	}

	public function label(): string
	{
		return $this->label;
	}
}

$wakeful = new WakefulSerializedDateTime('2026-06-15 12:15:00.987654', new DateTimeZone('UTC'), 'fallback');

var_dump(OPcache\VolatileCache::getInstance('default')->store('wakeful_datetime', $wakeful));

$wakefulCopy = OPcache\VolatileCache::getInstance('default')->fetch('wakeful_datetime');
var_dump($wakefulCopy instanceof WakefulSerializedDateTime);
var_dump($wakefulCopy->format('Y-m-d H:i:s.u e'));
var_dump($wakefulCopy->label());
var_dump(WakefulSerializedDateTime::$serializeCount);
var_dump(WakefulSerializedDateTime::$unserializeCount);

?>
--EXPECT--
bool(true)
bool(true)
string(30) "2026-06-15 11:00:00.111111 UTC"
bool(true)
bool(true)
string(35) "2023-10-27 10:00:00 +05:30 GMT+0530"
string(6) "+05:30"
bool(true)
bool(true)
string(30) "2023-10-27 10:00:00 -05:00 EST"
string(3) "EST"
bool(true)
bool(true)
string(39) "2026-06-15 09:30:00.123456 Europe/Paris"
string(8) "launch:7"
bool(true)
bool(true)
string(30) "2026-06-15 10:45:00.654321 UTC"
string(8) "fallback"
int(0)
int(0)
bool(true)
bool(true)
string(30) "2026-06-15 12:15:00.987654 UTC"
string(8) "fallback"
int(1)
int(1)
