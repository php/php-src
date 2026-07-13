--TEST--
UserCache\Cache: DatePeriod round-trips through its serialization contract
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
$cache = UserCache\Cache::getPool('dateperiod');

function ok(string $label, bool $cond): void
{
	echo $label, ': ', $cond ? 'OK' : 'FAIL', "\n";
}

function period_dates(DatePeriod $period): array
{
	$dates = [];
	foreach ($period as $date) {
		$dates[] = $date->format('Y-m-d');
	}
	return $dates;
}

/* Compare before iteration mutates the DatePeriod cursor. */
$byRecurrences = new DatePeriod(new DateTimeImmutable('2026-01-01'), new DateInterval('P1D'), 3);
$cache->store('recurrences', $byRecurrences);
$fetched = $cache->fetch('recurrences');
ok('recurrences instanceof', $fetched instanceof DatePeriod);
ok('recurrences parity', serialize($fetched) === serialize($byRecurrences));
ok('recurrences dates', period_dates($fetched) === ['2026-01-01', '2026-01-02', '2026-01-03', '2026-01-04']);

$byEnd = new DatePeriod(
	new DateTimeImmutable('2026-03-01'),
	new DateInterval('P1M'),
	new DateTimeImmutable('2026-05-15'),
);
$cache->store('end', $byEnd);
$fetched = $cache->fetch('end');
ok('end parity', serialize($fetched) === serialize($byEnd));
ok('end dates', period_dates($fetched) === ['2026-03-01', '2026-04-01', '2026-05-01']);

$excludeStart = new DatePeriod(
	new DateTimeImmutable('2026-01-01'),
	new DateInterval('P1D'),
	2,
	DatePeriod::EXCLUDE_START_DATE,
);
$cache->store('exclude', $excludeStart);
$fetched = $cache->fetch('exclude');
ok('exclude-start parity', serialize($fetched) === serialize($excludeStart));
ok('exclude-start dates', period_dates($fetched) === ['2026-01-02', '2026-01-03']);

$interval = new DateInterval('P2D');
$graph = [
	'period' => new DatePeriod(new DateTimeImmutable('2026-06-01'), $interval, 2),
	'interval' => $interval,
	'zone' => new DateTimeZone('Asia/Tokyo'),
];
$cache->store('graph', $graph);
$fetched = $cache->fetch('graph');
ok('graph parity', serialize($fetched) === serialize($graph));
ok('nested components', $fetched['interval'] instanceof DateInterval && $fetched['zone']->getName() === 'Asia/Tokyo');
ok('nested period dates', period_dates($fetched['period']) === ['2026-06-01', '2026-06-03', '2026-06-05']);
?>
--EXPECT--
recurrences instanceof: OK
recurrences parity: OK
recurrences dates: OK
end parity: OK
end dates: OK
exclude-start parity: OK
exclude-start dates: OK
graph parity: OK
nested components: OK
nested period dates: OK
