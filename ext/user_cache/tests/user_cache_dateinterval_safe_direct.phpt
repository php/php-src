--TEST--
UserCache\Cache: DateInterval safe-direct state round-trips (incl. subclasses)
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
date.timezone=UTC
--FILE--
<?php

class TaggedInterval extends DateInterval
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

$cache = UserCache\Cache::getPool('dateinterval-safe-direct');
$cache->clear();

/* --- spec interval: non-from_string, full field set --- */
$spec = new DateInterval('P1Y2M3DT4H5M6S');
$cache->store('spec', $spec);
$s = $cache->fetch('spec');
var_dump($s instanceof DateInterval);
var_dump(serialize($s) === serialize($spec));
var_dump($s->format('%y-%m-%d %h:%i:%s'));

/* --- diff interval: carries computed days + invert --- */
$diff = (new DateTimeImmutable('2026-01-01 00:00:00'))->diff(new DateTimeImmutable('2026-03-15 10:30:00'));
$cache->store('diff', $diff);
$d = $cache->fetch('diff');
var_dump(serialize($d) === serialize($diff));
var_dump($d->days);
var_dump($d->invert);
var_dump($d->format('%a days %h:%i'));

/* --- inverted diff --- */
$inv = (new DateTimeImmutable('2026-03-15'))->diff(new DateTimeImmutable('2026-01-01'));
$cache->store('inv', $inv);
var_dump($cache->fetch('inv')->invert);

/* --- from_string interval --- */
$fs = DateInterval::createFromDateString('2 days 4 hours');
$cache->store('fs', $fs);
$f = $cache->fetch('fs');
var_dump(serialize($f) === serialize($fs));
var_dump($f->format('%d %h'));

/* --- relative-spec from_string interval (special/weekday rel fields) --- */
$rel = DateInterval::createFromDateString('last day of next month');
$cache->store('rel', $rel);
$rl = $cache->fetch('rel');
var_dump(serialize($rl) === serialize($rel));
$base = new DateTimeImmutable('2026-07-23');
var_dump($base->add($rl)->format('Y-m-d') === $base->add($rel)->format('Y-m-d'));

/* --- subclass with extra state, no __serialize override => safe-direct --- */
$tagged = new TaggedInterval('P10DT12H');
$tagged->tag('window', 9);
$cache->store('tagged', $tagged);
$t = $cache->fetch('tagged');
var_dump($t instanceof TaggedInterval);
var_dump($t->label);
var_dump($t->describe());
var_dump($t->format('%d %h'));
?>
--EXPECT--
bool(true)
bool(true)
string(11) "1-2-3 4:5:6"
bool(true)
int(73)
int(0)
string(13) "73 days 10:30"
int(1)
bool(true)
string(3) "2 4"
bool(true)
bool(true)
bool(true)
string(6) "window"
string(8) "window:9"
string(5) "10 12"
