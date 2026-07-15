--TEST--
UserCache\Cache: DatePeriod safe-direct state round-trips (incl. subclasses)
--EXTENSIONS--
user_cache
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
date.timezone=UTC
--FILE--
<?php

class TaggedDatePeriod extends DatePeriod
{
	public string $label = 'default';
	private int $revision = 0;

	public function tag(string $label, int $revision): void
	{
		$this->label = $label;
		$this->revision = $revision;
	}

	public function describe(): string
	{
		return $this->label . ':' . $this->revision;
	}
}

class MagicDatePeriod extends DatePeriod
{
	public static int $serializeCount = 0;
	public static int $unserializeCount = 0;

	public string $note = 'none';

	public function __serialize(): array
	{
		self::$serializeCount++;

		return parent::__serialize() + ['note' => 'serialized-' . $this->note];
	}

	public function __unserialize(array $data): void
	{
		self::$unserializeCount++;
		parent::__unserialize($data);
		$this->note = $data['note'];
	}
}

function period_dates(DatePeriod $period): array
{
	$dates = [];
	foreach ($period as $date) {
		$dates[] = $date->format('Y-m-d');
	}
	return $dates;
}

$cache = UserCache\Cache::getPool('dateperiod-safe-direct');
$cache->clear();

/* --- recurrences-based --- */
$byRecurrences = new DatePeriod(new DateTimeImmutable('2026-01-01'), new DateInterval('P1D'), 3);
$cache->store('recurrences', $byRecurrences);
$r = $cache->fetch('recurrences');
var_dump($r instanceof DatePeriod);
var_dump(serialize($r) === serialize($byRecurrences));
var_dump(period_dates($r) === ['2026-01-01', '2026-01-02', '2026-01-03', '2026-01-04']);

/* --- end-based --- */
$byEnd = new DatePeriod(
	new DateTimeImmutable('2026-03-01'),
	new DateInterval('P1M'),
	new DateTimeImmutable('2026-05-15'),
);
$cache->store('end', $byEnd);
var_dump(serialize($cache->fetch('end')) === serialize($byEnd));

/* --- EXCLUDE_START_DATE --- */
$excludeStart = new DatePeriod(
	new DateTimeImmutable('2026-01-01'),
	new DateInterval('P1D'),
	2,
	DatePeriod::EXCLUDE_START_DATE,
);
$cache->store('exclude-start', $excludeStart);
var_dump(period_dates($cache->fetch('exclude-start')) === ['2026-01-02', '2026-01-03']);

/* --- INCLUDE_END_DATE --- */
$includeEnd = new DatePeriod(
	new DateTimeImmutable('2026-01-01'),
	new DateInterval('P1D'),
	new DateTimeImmutable('2026-01-03'),
	DatePeriod::INCLUDE_END_DATE,
);
$cache->store('include-end', $includeEnd);
var_dump(serialize($cache->fetch('include-end')) === serialize($includeEnd));
var_dump(period_dates($cache->fetch('include-end')) === ['2026-01-01', '2026-01-02', '2026-01-03']);

/* --- subclass with extra state, no __serialize override => safe-direct --- */
$tagged = new TaggedDatePeriod(new DateTimeImmutable('2026-02-01'), new DateInterval('P1D'), 2);
$tagged->tag('window', 9);
$cache->store('tagged', $tagged);
$t = $cache->fetch('tagged');
var_dump($t instanceof TaggedDatePeriod);
var_dump($t->label);
var_dump($t->describe());
var_dump(period_dates($t) === ['2026-02-01', '2026-02-02', '2026-02-03']);

/* --- subclass overriding __serialize => magic hooks route, still round-trips --- */
$magic = new MagicDatePeriod(new DateTimeImmutable('2026-04-01'), new DateInterval('P1D'), 1);
$magic->note = 'hello';
$cache->store('magic', $magic);
$m = $cache->fetch('magic');
var_dump($m instanceof MagicDatePeriod);
var_dump($m->note);
var_dump(period_dates($m) === ['2026-04-01', '2026-04-02']);
var_dump(MagicDatePeriod::$serializeCount > 0);
var_dump(MagicDatePeriod::$unserializeCount > 0);

/* --- shared identity: interval reused across the graph --- */
$interval = new DateInterval('P2D');
$graph = [
	'period' => new DatePeriod(new DateTimeImmutable('2026-06-01'), $interval, 2),
	'interval' => $interval,
	'zone' => new DateTimeZone('Asia/Tokyo'),
];
$cache->store('graph', $graph);
$g = $cache->fetch('graph');
var_dump(serialize($g) === serialize($graph));
var_dump($g['interval'] instanceof DateInterval && $g['zone']->getName() === 'Asia/Tokyo');
var_dump(period_dates($g['period']) === ['2026-06-01', '2026-06-03', '2026-06-05']);
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
string(6) "window"
string(8) "window:9"
bool(true)
bool(true)
string(5) "hello"
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
