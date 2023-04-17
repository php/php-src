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

try {
	$dp = new DatePeriod("2023-01-13 17:24:58", DateInterval::createFromDateString("tomorrow"), 4);
	echo "OK\n";
} catch (\TypeError $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
OK
OK
OK
TypeError: DatePeriod::__construct() accepts (DateTimeInterface, DateInterval, int [, int]), or (DateTimeInterface, DateInterval, DateTime [, int]), or (string [, int]) as arguments
