--TEST--
DatePeriod modify readonly property
--FILE--
<?php
$dp = new \DatePeriod(
	new \DateTimeImmutable("2023-01-13"),
	DateInterval::createFromDateString('+1 month'),
	new \DateTimeImmutable("2023-12-31"),
);

try {
	$dp->interval = "foo";
} catch( \Error $e ) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
	$foo =& $dp->interval;
} catch( \Error $e ) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECTF--
Error: Cannot modify readonly property DatePeriod::$interval
Error: Cannot modify readonly property DatePeriod::$interval
