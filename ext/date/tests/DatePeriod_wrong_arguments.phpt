--TEST--
DatePeriod arguments/wrong arguments
--FILE--
<?php
$dp = new DatePeriod(new \DateTimeImmutable("2023-01-13 12:29:30"), DateInterval::createFromDateString("tomorrow"), 4);
echo get_class($dp) == 'DatePeriod' ? "OK\n" : "FAIL\n";

$dp = new DatePeriod(new \DateTimeImmutable("2023-01-13 12:29:30"), DateInterval::createFromDateString("tomorrow"), new \DateTimeImmutable("2023-01-16 16:49:29"));
echo get_class($dp) == 'DatePeriod' ? "OK\n" : "FAIL\n";

$dp = new DatePeriod("R4/2012-07-01T00:00:00Z/P7D");
echo get_class($dp) == 'DatePeriod' ? "OK\n" : "FAIL\n";

$dp = DatePeriod::createFromISO8601String("R4/2012-07-01T00:00:00Z/P7D");
echo get_class($dp) == 'DatePeriod' ? "OK\n" : "FAIL\n";

try {
	$dp = new DatePeriod("2023-01-13 17:24:58", DateInterval::createFromDateString("tomorrow"), 4);
	echo "OK\n";
} catch (\TypeError $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECTF--
OK
OK

Deprecated: Calling DatePeriod::__construct(string $isostr, int $options = 0) is deprecated, use DatePeriod::createFromISO8601String() instead in %s on line %d
OK
OK
TypeError: DatePeriod::__construct() accepts (DateTimeInterface, DateInterval, int [, int]), or (DateTimeInterface, DateInterval, DateTime [, int]), or (string [, int]) as arguments
