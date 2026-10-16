--TEST--
UserCache\Cache: DateTime and DateInterval safe-direct state is restored for subclasses and diffs
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=16M
date.timezone=UTC
--FILE--
<?php
/* DateTime, DateTimeZone and DateInterval subclasses restore state, hooks and object ids */
class UserCacheCarbonLikeDateTime extends DateTimeImmutable
{
    public string $label = 'default';
}

class UserCacheDateModel
{
    public function __construct(
        public UserCacheCarbonLikeDateTime $createdAt,
        public UserCacheCarbonLikeDateTime $updatedAt,
        public UserCacheCarbonLikeDateTime $deletedAt,
    ) {
    }
}

class UserCacheSelfIdDateTime extends DateTime
{
    private int $cachedSelfId;
    public int $publicSelfId;

    public function __construct(string $time, DateTimeZone $timezone)
    {
        parent::__construct($time, $timezone);
        $this->cachedSelfId = spl_object_id($this);
        $this->publicSelfId = spl_object_id($this);
    }

    public function cachedSelfId(): int
    {
        return $this->cachedSelfId;
    }
}

class UserCacheMagicDateTime extends DateTime
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

        return parent::__serialize() + ['label' => 'serialized-' . $this->label];
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

class UserCacheMagicSelfIdDateTime extends DateTime
{
    public static int $serializeCount = 0;
    public static int $unserializeCount = 0;

    private int $constructedObjectId;

    public function __construct(string $time, DateTimeZone $timezone)
    {
        parent::__construct($time, $timezone);
        $this->constructedObjectId = spl_object_id($this);
    }

    public function __serialize(): array
    {
        self::$serializeCount++;

        return parent::__serialize() + ['constructedObjectId' => $this->constructedObjectId];
    }

    public function __unserialize(array $data): void
    {
        self::$unserializeCount++;
        parent::__unserialize($data);
        $this->constructedObjectId = spl_object_id($this);
    }

    public function constructedObjectId(): int
    {
        return $this->constructedObjectId;
    }
}

class UserCacheMagicFilteredDateTime extends DateTime
{
    public static int $serializeCount = 0;
    public static int $unserializeCount = 0;

    public Closure $hidden;
    private string $label;

    public function __construct(string $time, DateTimeZone $timezone, string $label)
    {
        parent::__construct($time, $timezone);
        $this->hidden = static fn(): int => 1;
        $this->label = $label;
    }

    public function __serialize(): array
    {
        self::$serializeCount++;
        $timezone = $this->getTimezone();

        return [
            'date' => $this->format('Y-m-d H:i:s.u'),
            'timezone_type' => 3,
            'timezone' => $timezone->getName(),
            'label' => $this->label,
        ];
    }

    public function __unserialize(array $data): void
    {
        self::$unserializeCount++;
        parent::__unserialize($data);
        $this->hidden = static fn(): int => 2;
        $this->label = $data['label'];
    }

    public function label(): string
    {
        return $this->label;
    }
}

class UserCacheWakefulDateTime extends DateTime
{
    public static int $sleepCount = 0;
    public static int $wakeupCount = 0;

    private string $label;

    public function __construct(string $time, DateTimeZone $timezone, string $label)
    {
        parent::__construct($time, $timezone);
        $this->label = $label;
    }

    public function __sleep(): array
    {
        self::$sleepCount++;

        return ['label'];
    }

    public function __wakeup(): void
    {
        self::$wakeupCount++;
    }

    public function label(): string
    {
        return $this->label;
    }
}

class UserCacheTaggedTimeZone extends DateTimeZone
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

class UserCacheTaggedInterval extends DateInterval
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

$cache = UserCache\Cache::getPool('datetime-safe-direct');

$date = new UserCacheCarbonLikeDateTime('2024-01-02 03:04:05.123456', new DateTimeZone('Asia/Tokyo'));
$date->label = 'tokyo';

$model = new UserCacheDateModel(
    new UserCacheCarbonLikeDateTime('2026-06-29 09:00:00.000001', new DateTimeZone('UTC')),
    new UserCacheCarbonLikeDateTime('2026-06-29 09:30:00.000002', new DateTimeZone('Europe/Paris')),
    new UserCacheCarbonLikeDateTime('2026-06-29 10:00:00.000003', new DateTimeZone('America/New_York')),
);

$payload = [
    'date' => $date,
    'model' => $model,
    'offset' => new DateTimeImmutable('2023-10-27 10:00:00.000001 +05:30'),
    'abbr' => new DateTimeImmutable('2023-10-27 10:00:00.000002 EST'),
    'timezone' => new DateTimeZone('Europe/Paris'),
    'interval' => new DateInterval('P1DT2H'),
    'taggedTimezone' => new UserCacheTaggedTimeZone('Europe/Paris', 'paris'),
    'taggedInterval' => new UserCacheTaggedInterval('P1Y2M3DT4H5M6S', 'window', 9),
    'relativeInterval' => DateInterval::createFromDateString('2 days 4 hours'),
    'selfId' => new UserCacheSelfIdDateTime('2026-06-15 10:15:00.333333', new DateTimeZone('UTC')),
    'magicDate' => new UserCacheMagicDateTime('2026-06-15 10:45:00.654321', new DateTimeZone('UTC'), 'magic'),
    'magicSelfId' => new UserCacheMagicSelfIdDateTime('2026-06-15 11:45:00.111111', new DateTimeZone('UTC')),
    'magicFiltered' => new UserCacheMagicFilteredDateTime('2026-06-15 12:00:00.222222', new DateTimeZone('UTC'), 'filtered'),
    'wakefulDate' => new UserCacheWakefulDateTime('2026-06-15 12:15:00.987654', new DateTimeZone('UTC'), 'wakeful'),
];

var_dump($cache->store('payload', $payload));

$fetched = $cache->fetch('payload');

var_dump($fetched['date'] instanceof UserCacheCarbonLikeDateTime);
var_dump($fetched['date']->label);
var_dump($fetched['date']->format('Y-m-d H:i:s.u P e'));

var_dump($fetched['model'] instanceof UserCacheDateModel);
var_dump($fetched['model']->createdAt->format('Y-m-d H:i:s.u e'));
var_dump($fetched['model']->updatedAt->format('Y-m-d H:i:s.u e'));
var_dump($fetched['model']->deletedAt->format('Y-m-d H:i:s.u e'));

var_dump($fetched['offset']->format('Y-m-d H:i:s.u P e'));
var_dump($fetched['abbr']->format('Y-m-d H:i:s.u P e'));
var_dump($fetched['timezone']->getName());
var_dump($fetched['interval']->format('%d %h'));
var_dump($fetched['taggedTimezone'] instanceof UserCacheTaggedTimeZone);
var_dump($fetched['taggedTimezone']->getName());
var_dump($fetched['taggedTimezone']->label());
var_dump($fetched['taggedInterval'] instanceof UserCacheTaggedInterval);
var_dump($fetched['taggedInterval']->format('%y-%m-%d %h:%i:%s'));
var_dump($fetched['taggedInterval']->describe());
var_dump($fetched['relativeInterval'] instanceof DateInterval);
var_dump($fetched['relativeInterval']->format('%d %h'));

var_dump($fetched['selfId'] instanceof UserCacheSelfIdDateTime);
var_dump($fetched['selfId']->cachedSelfId() === $payload['selfId']->cachedSelfId());
var_dump($fetched['selfId']->cachedSelfId() !== spl_object_id($fetched['selfId']));
var_dump($fetched['selfId']->publicSelfId === spl_object_id($payload['selfId']));
var_dump($fetched['selfId']->publicSelfId === spl_object_id($fetched['selfId']));

var_dump($fetched['magicDate'] instanceof UserCacheMagicDateTime);
var_dump($fetched['magicDate']->format('Y-m-d H:i:s.u e'));
var_dump($fetched['magicDate']->label());
var_dump(UserCacheMagicDateTime::$serializeCount);
var_dump(UserCacheMagicDateTime::$unserializeCount);

var_dump($fetched['magicSelfId'] instanceof UserCacheMagicSelfIdDateTime);
var_dump($fetched['magicSelfId']->format('Y-m-d H:i:s.u e'));
var_dump($fetched['magicSelfId']->constructedObjectId() === spl_object_id($fetched['magicSelfId']));
var_dump($fetched['magicSelfId']->constructedObjectId() !== spl_object_id($payload['magicSelfId']));
var_dump(UserCacheMagicSelfIdDateTime::$serializeCount);
var_dump(UserCacheMagicSelfIdDateTime::$unserializeCount);

var_dump($fetched['magicFiltered'] instanceof UserCacheMagicFilteredDateTime);
var_dump($fetched['magicFiltered']->format('Y-m-d H:i:s.u e'));
var_dump($fetched['magicFiltered']->label());
var_dump(($fetched['magicFiltered']->hidden)());
var_dump(UserCacheMagicFilteredDateTime::$serializeCount);
var_dump(UserCacheMagicFilteredDateTime::$unserializeCount);

var_dump($fetched['wakefulDate'] instanceof UserCacheWakefulDateTime);
var_dump($fetched['wakefulDate']->format('Y-m-d H:i:s.u e'));
var_dump($fetched['wakefulDate']->label());
var_dump(UserCacheWakefulDateTime::$sleepCount);
var_dump(UserCacheWakefulDateTime::$wakeupCount);

/* DateInterval from diff() and relative strings keeps days, invert and special fields */
$cache = UserCache\Cache::getPool('datetime-interval-diff');

$diff = (new DateTimeImmutable('2026-01-01 00:00:00'))->diff(new DateTimeImmutable('2026-03-15 10:30:00'));
$cache->store('diff', $diff);
$d = $cache->fetch('diff');
var_dump(serialize($d) === serialize($diff));
var_dump($d->days);
var_dump($d->invert);
var_dump($d->format('%a days %h:%i'));

$inv = (new DateTimeImmutable('2026-03-15'))->diff(new DateTimeImmutable('2026-01-01'));
$cache->store('inv', $inv);
var_dump($cache->fetch('inv')->invert);

$rel = DateInterval::createFromDateString('last day of next month');
$cache->store('rel', $rel);
$rl = $cache->fetch('rel');
var_dump(serialize($rl) === serialize($rel));
$base = new DateTimeImmutable('2026-07-23');
var_dump($base->add($rl)->format('Y-m-d') === $base->add($rel)->format('Y-m-d'));

/* Validated restore keeps parity with native unserialize(serialize()) for edge values */
echo "\ndatetime edge values:\n";
$cache = UserCache\Cache::getPool('datetime-edge-values');
$cache->clear();

function edge(string $label, DateTimeInterface $value): void
{
    global $cache;
    $cache->store($label, $value);
    $fetched = $cache->fetch($label, 'unrestorable');
    $native = unserialize(serialize($value));
    echo $label, ': ';
    if (!$fetched instanceof DateTimeInterface) {
        var_dump($fetched);
        return;
    }
    echo serialize($fetched) === serialize($native) ? 'serialize-same' : 'serialize-differs';
    echo ' ', $fetched == $native ? 'same-instant' : 'other-instant';
    echo ' ', $fetched->format('Y-m-d H:i:s.u T P U'), "\n";
}

$newYork = new DateTimeZone('America/New_York');
$utc = new DateTimeZone('UTC');
edge('max-year', new DateTimeImmutable('@9223372036854775807'));
edge('min-year', new DateTimeImmutable('@-9223372036854775808'));
edge('negative-year', new DateTimeImmutable('-0044-03-15 12:00:00', $utc));
edge('year-100000', new DateTimeImmutable('+100000-01-01 00:00:00', new DateTimeZone('Europe/Paris')));
edge('microseconds-max', new DateTimeImmutable('2024-12-31 23:59:59.999999', $utc));
edge('abbr-dst', new DateTimeImmutable('2024-07-01 12:00:00 EDT'));
edge('abbr-set-timezone', (new DateTimeImmutable('2024-01-01 12:00:00 UTC'))->setTimezone(new DateTimeZone('CEST')));
edge('offset-negative', new DateTimeImmutable('2024-07-01 12:00:00 -03:30'));
edge('dst-fall-back-first', new DateTimeImmutable('2024-11-03 01:30:00', $newYork));
edge('dst-spring-gap-settime', (new DateTime('2024-03-10 00:00:00', $newYork))->setTime(2, 30));
edge('add-weekday-relative', (new DateTimeImmutable('2026-01-01 10:00:00', $utc))->add(DateInterval::createFromDateString('next monday')));
edge('add-special-relative', (new DateTimeImmutable('2026-01-01 10:00:00', $utc))->add(DateInterval::createFromDateString('first monday of next month')));
edge('add-weekdays', (new DateTimeImmutable('2026-01-01 10:00:00', $utc))->add(DateInterval::createFromDateString('+3 weekdays')));
edge('modify-last-day-of', (new DateTime('2026-01-31 10:00:00', $newYork))->modify('last day of next month'));
edge('modify-this-week', (new DateTime('2026-01-01 10:00:00', $newYork))->modify('monday this week'));
edge('set-iso-date', (new DateTime('2026-01-01 10:00:00', $newYork))->setISODate(2026, 53, 7));
edge('from-format-u', DateTime::createFromFormat('U.u', '1700000000.123456'));

/* The stored instant survives where native serialization is lossy: the second 01:30 of a DST fall-back, and a +99:59 offset */
$fallBackSecond = (new DateTimeImmutable('now', $newYork))->setTimestamp(1730615400);
$cache->store('dst-fall-back-second', $fallBackSecond);
var_dump($cache->fetch('dst-fall-back-second')->format('Y-m-d H:i:s T U'));
var_dump(unserialize(serialize($fallBackSecond))->format('Y-m-d H:i:s T U'));
$wideOffset = (new DateTimeImmutable('2024-07-01 12:00:00 UTC'))->setTimezone(new DateTimeZone('+99:59'));
$cache->store('offset-wide', $wideOffset);
var_dump($cache->fetch('offset-wide')->format('Y-m-d H:i:s P U') === $wideOffset->format('Y-m-d H:i:s P U'));
try {
    unserialize(serialize($wideOffset));
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

/* DateInterval restore accepts every field combination native construction and unserialize() produce */
echo "\ninterval edge values:\n";
function interval_edge(string $label, DateInterval $interval): void
{
    global $cache;
    $cache->store($label, $interval);
    $fetched = $cache->fetch($label, 'unrestorable');
    echo $label, ': ';
    if (!$fetched instanceof DateInterval) {
        var_dump($fetched);
        return;
    }
    $base = new DateTimeImmutable('2026-07-23 08:00:00', new DateTimeZone('UTC'));
    echo serialize($fetched) === serialize($interval) ? 'serialize-same' : 'serialize-differs';
    echo ' ', $base->add($fetched) == $base->add($interval) ? 'adds-same' : 'adds-differently';
    echo ' ', $fetched->format('%R %y-%m-%d %h:%i:%s.%f %a'), "\n";
}
interval_edge('iso', new DateInterval('P1Y2M3DT4H5M6S'));
interval_edge('negative-components', DateInterval::createFromDateString('-2 days -3 hours'));
interval_edge('first-weekday-of', DateInterval::createFromDateString('first monday of next month'));
interval_edge('last-weekday-of', DateInterval::createFromDateString('last friday of this month'));
interval_edge('weekdays', DateInterval::createFromDateString('+3 weekdays'));
interval_edge('next-weekday', DateInterval::createFromDateString('next sunday'));
interval_edge('last-weekday', DateInterval::createFromDateString('last sunday'));
interval_edge('this-week', DateInterval::createFromDateString('monday this week'));
interval_edge('diff-inverted-microseconds', (new DateTimeImmutable('2026-03-15 10:00:00.5'))->diff(new DateTimeImmutable('2026-01-01')));
interval_edge('from-period', (new DatePeriod(new DateTimeImmutable('2026-01-01'), new DateInterval('P1D'), 2))->getDateInterval());
interval_edge('unserialized', unserialize(serialize(new DateInterval('P1D'))));
interval_edge('set-state', DateInterval::__set_state(['y' => 1, 'm' => 2, 'd' => 3, 'h' => 4, 'i' => 5, 's' => 6, 'f' => 0.5, 'invert' => 1, 'days' => false]));
interval_edge('set-state-empty', DateInterval::__set_state([]));
?>
--EXPECT--
bool(true)
bool(true)
string(5) "tokyo"
string(44) "2024-01-02 03:04:05.123456 +09:00 Asia/Tokyo"
bool(true)
string(30) "2026-06-29 09:00:00.000001 UTC"
string(39) "2026-06-29 09:30:00.000002 Europe/Paris"
string(43) "2026-06-29 10:00:00.000003 America/New_York"
string(40) "2023-10-27 10:00:00.000001 +05:30 +05:30"
string(37) "2023-10-27 10:00:00.000002 -05:00 EST"
string(12) "Europe/Paris"
string(3) "1 2"
bool(true)
string(12) "Europe/Paris"
string(5) "paris"
bool(true)
string(11) "1-2-3 4:5:6"
string(8) "window:9"
bool(true)
string(3) "2 4"
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
bool(true)
string(30) "2026-06-15 10:45:00.654321 UTC"
string(16) "serialized-magic"
int(1)
int(1)
bool(true)
string(30) "2026-06-15 11:45:00.111111 UTC"
bool(true)
bool(true)
int(1)
int(1)
bool(true)
string(30) "2026-06-15 12:00:00.222222 UTC"
string(8) "filtered"
int(2)
int(1)
int(1)
bool(true)
string(30) "2026-06-15 12:15:00.987654 UTC"
string(7) "wakeful"
int(0)
int(0)
bool(true)
int(73)
int(0)
string(13) "73 days 10:30"
int(1)
bool(true)
bool(true)

datetime edge values:
max-year: serialize-same same-instant 292277026596-12-04 15:30:07.000000 GMT+0000 +00:00 9223372036854775807
min-year: serialize-same same-instant -292277022657-01-27 08:29:52.000000 GMT+0000 +00:00 -9223372036854775808
negative-year: serialize-same same-instant -0044-03-15 12:00:00.000000 UTC +00:00 -63549316800
year-100000: serialize-same same-instant 100000-01-01 00:00:00.000000 CET +01:00 3093527977200
microseconds-max: serialize-same same-instant 2024-12-31 23:59:59.999999 UTC +00:00 1735689599
abbr-dst: serialize-same same-instant 2024-07-01 12:00:00.000000 EDT -04:00 1719849600
abbr-set-timezone: serialize-same same-instant 2024-01-01 14:00:00.000000 CEST +02:00 1704110400
offset-negative: serialize-same same-instant 2024-07-01 12:00:00.000000 GMT-0330 -03:30 1719847800
dst-fall-back-first: serialize-same same-instant 2024-11-03 01:30:00.000000 EDT -04:00 1730611800
dst-spring-gap-settime: serialize-same same-instant 2024-03-10 03:30:00.000000 EDT -04:00 1710055800
add-weekday-relative: serialize-same same-instant 2026-01-05 10:00:00.000000 UTC +00:00 1767607200
add-special-relative: serialize-same same-instant 2026-02-02 10:00:00.000000 UTC +00:00 1770026400
add-weekdays: serialize-same same-instant 2026-01-06 10:00:00.000000 UTC +00:00 1767693600
modify-last-day-of: serialize-same same-instant 2026-02-28 10:00:00.000000 EST -05:00 1772290800
modify-this-week: serialize-same same-instant 2025-12-29 00:00:00.000000 EST -05:00 1766984400
set-iso-date: serialize-same same-instant 2027-01-03 10:00:00.000000 EST -05:00 1798988400
from-format-u: serialize-same same-instant 2023-11-14 22:13:20.123456 GMT+0000 +00:00 1700000000
string(34) "2024-11-03 01:30:00 EST 1730615400"
string(34) "2024-11-03 01:30:00 EDT 1730611800"
bool(true)
Invalid serialization data for DateTimeImmutable object

interval edge values:
iso: serialize-same adds-same + 1-2-3 4:5:6.0 (unknown)
negative-components: serialize-same adds-same + 0-0--2 -3:0:0.0 (unknown)
first-weekday-of: serialize-same adds-same + 0-1-0 0:0:0.0 (unknown)
last-weekday-of: serialize-same adds-same + 0-0--7 0:0:0.0 (unknown)
weekdays: serialize-same adds-same + 0-0-0 0:0:0.0 (unknown)
next-weekday: serialize-same adds-same + 0-0-0 0:0:0.0 (unknown)
last-weekday: serialize-same adds-same + 0-0--7 0:0:0.0 (unknown)
this-week: serialize-same adds-same + 0-0-0 0:0:0.0 (unknown)
diff-inverted-microseconds: serialize-same adds-same - 0-2-14 10:0:0.500000 73
from-period: serialize-same adds-same + 0-0-1 0:0:0.0 (unknown)
unserialized: serialize-same adds-same + 0-0-1 0:0:0.0 (unknown)
set-state: serialize-same adds-same - 1-2-3 4:5:6.500000 (unknown)
set-state-empty: serialize-same adds-same + -1--1--1 -1:-1:-1.0 -1
