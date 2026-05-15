--TEST--
OPcache __DirectCacheSafe covers DateTimeZone and DateInterval direct paths
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=32
--FILE--
<?php

var_dump(count((new ReflectionClass(DateTimeZone::class))->getAttributes(OPcache\__DirectCacheSafe::class)));
var_dump(count((new ReflectionClass(DateInterval::class))->getAttributes(OPcache\__DirectCacheSafe::class)));

class TaggedTimeZone extends DateTimeZone
{
	private string $label;

	public function __construct(string $timezone, string $label)
	{
		parent::__construct($timezone);
		$this->label = $label;
	}

	public function label(): string
	{
		return $this->label;
	}
}

$timezone = new TaggedTimeZone('Europe/Paris', 'paris');

var_dump(OPcache\volatile_store('safe_direct_timezone', $timezone));

$timezoneCopy = OPcache\volatile_fetch('safe_direct_timezone');
var_dump($timezoneCopy instanceof TaggedTimeZone);
var_dump($timezoneCopy->getName());
var_dump($timezoneCopy->label());

$offsetTimezone = (new DateTimeImmutable('2023-10-27 10:00:00 +05:30'))->getTimezone();

var_dump(OPcache\volatile_store('safe_direct_offset_timezone', $offsetTimezone));

$offsetTimezoneCopy = OPcache\volatile_fetch('safe_direct_offset_timezone');
var_dump($offsetTimezoneCopy instanceof DateTimeZone);
var_dump($offsetTimezoneCopy->getName());

$abbrTimezone = (new DateTimeImmutable('2023-10-27 10:00:00 EST'))->getTimezone();

var_dump(OPcache\volatile_store('safe_direct_abbr_timezone', $abbrTimezone));

$abbrTimezoneCopy = OPcache\volatile_fetch('safe_direct_abbr_timezone');
var_dump($abbrTimezoneCopy instanceof DateTimeZone);
var_dump($abbrTimezoneCopy->getName());

class TaggedInterval extends DateInterval
{
	private string $label;
	protected int $revision;

	public function __construct(string $duration, string $label, int $revision)
	{
		parent::__construct($duration);
		$this->label = $label;
		$this->revision = $revision;
	}

	public function describe(): string
	{
		return $this->label . ':' . $this->revision;
	}
}

$interval = new TaggedInterval('P1Y2M3DT4H5M6S', 'window', 9);

var_dump(OPcache\volatile_store('safe_direct_interval', $interval));

$intervalCopy = OPcache\volatile_fetch('safe_direct_interval');
var_dump($intervalCopy instanceof TaggedInterval);
var_dump($intervalCopy->format('%y-%m-%d %h:%i:%s'));
var_dump($intervalCopy->describe());

$relativeInterval = DateInterval::createFromDateString('2 days 4 hours');

var_dump(OPcache\volatile_store('safe_direct_relative_interval', $relativeInterval));

$relativeIntervalCopy = OPcache\volatile_fetch('safe_direct_relative_interval');
var_dump($relativeIntervalCopy instanceof DateInterval);
var_dump($relativeIntervalCopy->format('%d %h'));

?>
--EXPECT--
int(1)
int(1)
bool(true)
bool(true)
string(12) "Europe/Paris"
string(5) "paris"
bool(true)
bool(true)
string(6) "+05:30"
bool(true)
bool(true)
string(3) "EST"
bool(true)
bool(true)
string(11) "1-2-3 4:5:6"
string(8) "window:9"
bool(true)
bool(true)
string(3) "2 4"
