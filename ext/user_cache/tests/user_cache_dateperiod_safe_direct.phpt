--TEST--
UserCache\Cache: DatePeriod safe-direct state round-trips (incl. subclasses)
--INI--
user_cache.enable=1
user_cache.enable_cli=1
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

/* Compare serialization before iteration mutates the DatePeriod cursor. */
$byRecurrences = new DatePeriod(new DateTimeImmutable('2026-01-01'), new DateInterval('P1D'), 3);
$cache->store('recurrences', $byRecurrences);
$r = $cache->fetch('recurrences');
var_dump($r instanceof DatePeriod);
var_dump(serialize($r) === serialize($byRecurrences));
var_dump(period_dates($r) === ['2026-01-01', '2026-01-02', '2026-01-03', '2026-01-04']);

$byEnd = new DatePeriod(
    new DateTimeImmutable('2026-03-01'),
    new DateInterval('P1M'),
    new DateTimeImmutable('2026-05-15'),
);
$cache->store('end', $byEnd);
$r = $cache->fetch('end');
var_dump(serialize($r) === serialize($byEnd));
var_dump(period_dates($r) === ['2026-03-01', '2026-04-01', '2026-05-01']);

$excludeStart = new DatePeriod(
    new DateTimeImmutable('2026-01-01'),
    new DateInterval('P1D'),
    2,
    DatePeriod::EXCLUDE_START_DATE,
);
$cache->store('exclude-start', $excludeStart);
$r = $cache->fetch('exclude-start');
var_dump(serialize($r) === serialize($excludeStart));
var_dump(period_dates($r) === ['2026-01-02', '2026-01-03']);

$includeEnd = new DatePeriod(
    new DateTimeImmutable('2026-01-01'),
    new DateInterval('P1D'),
    new DateTimeImmutable('2026-01-03'),
    DatePeriod::INCLUDE_END_DATE,
);
$cache->store('include-end', $includeEnd);
var_dump(serialize($cache->fetch('include-end')) === serialize($includeEnd));
var_dump(period_dates($cache->fetch('include-end')) === ['2026-01-01', '2026-01-02', '2026-01-03']);

/* Inherited serialization uses the safe-direct route. */
$tagged = new TaggedDatePeriod(new DateTimeImmutable('2026-02-01'), new DateInterval('P1D'), 2);
$tagged->tag('window', 9);
$cache->store('tagged', $tagged);
$t = $cache->fetch('tagged');
var_dump($t instanceof TaggedDatePeriod);
var_dump($t->label);
var_dump($t->describe());
var_dump(period_dates($t) === ['2026-02-01', '2026-02-02', '2026-02-03']);

/* Overriding __serialize() selects the magic-hook route. */
$magic = new MagicDatePeriod(new DateTimeImmutable('2026-04-01'), new DateInterval('P1D'), 1);
$magic->note = 'hello';
$cache->store('magic', $magic);
$m = $cache->fetch('magic');
var_dump($m instanceof MagicDatePeriod);
var_dump($m->note);
var_dump(period_dates($m) === ['2026-04-01', '2026-04-02']);
var_dump(MagicDatePeriod::$serializeCount > 0);
var_dump(MagicDatePeriod::$unserializeCount > 0);

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

$offsetPeriod = new DatePeriod(
    new DateTimeImmutable('2026-01-01 00:00:00 +09:00'),
    new DateInterval('P1D'),
    new DateTimeImmutable('2026-01-04 00:00:00 +09:00'),
);
$cache->store('offset-tz', $offsetPeriod);
$o = $cache->fetch('offset-tz');
var_dump(serialize($o) === serialize($offsetPeriod));
var_dump(period_dates($o) === ['2026-01-01', '2026-01-02', '2026-01-03']);
var_dump($o->getStartDate()->getTimezone()->getName());

$abbrPeriod = new DatePeriod(
    new DateTimeImmutable('2026-01-01 00:00:00 PST'),
    new DateInterval('P1D'),
    2,
);
$cache->store('abbr-tz', $abbrPeriod);
$a = $cache->fetch('abbr-tz');
var_dump(serialize($a) === serialize($abbrPeriod));
var_dump($a->getStartDate()->getTimezone()->getName());

/* Iteration leaves cursor state to restore. */
$iterated = new DatePeriod(new DateTimeImmutable('2026-01-01'), new DateInterval('P1D'), 2);
foreach ($iterated as $unused) {
}
$cache->store('iterated', $iterated);
var_dump(serialize($cache->fetch('iterated')) === serialize($iterated));

$anonymousStart = new DatePeriod(new class('2020-01-01') extends DateTimeImmutable {}, new DateInterval('P1D'), 2);
try {
    $cache->store('anonymous-start', $anonymousStart);
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}
var_dump($cache->fetch('anonymous-start', 'not stored'));

/* Cursors advanced by an unserialize()d or weekday-relative interval carry that interval's stale relative fields and still restore. */
echo "\ncursor relative state:\n";
$unserializedInterval = unserialize(serialize(new DateInterval('P1D')));
$fromUnserialized = new DatePeriod(new DateTimeImmutable('2026-01-01'), $unserializedInterval, 2);
foreach ($fromUnserialized as $unused) {
}
$cache->store('cursor-unserialized-interval', $fromUnserialized);
$u = $cache->fetch('cursor-unserialized-interval', 'unrestorable');
var_dump($u instanceof DatePeriod && serialize($u) === serialize($fromUnserialized));
var_dump(period_dates($u) === ['2026-01-01', '2026-01-02', '2026-01-03']);

$byWeekday = new DatePeriod(new DateTimeImmutable('2026-01-01'), DateInterval::createFromDateString('next monday'), 2);
foreach ($byWeekday as $unused) {
}
$cache->store('cursor-weekday-interval', $byWeekday);
$w = $cache->fetch('cursor-weekday-interval', 'unrestorable');
var_dump($w instanceof DatePeriod && serialize($w) === serialize($byWeekday));
var_dump(period_dates($w) === period_dates($byWeekday), period_dates($w));
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
bool(true)
bool(true)
string(6) "+09:00"
bool(true)
string(3) "PST"
bool(true)
Serialization of 'DateTimeImmutable@anonymous' is not allowed
string(10) "not stored"

cursor relative state:
bool(true)
bool(true)
bool(true)
bool(true)
array(3) {
  [0]=>
  string(10) "2026-01-01"
  [1]=>
  string(10) "2026-01-05"
  [2]=>
  string(10) "2026-01-12"
}
