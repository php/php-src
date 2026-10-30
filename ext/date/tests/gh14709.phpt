--TEST--
Bug GH-14709 overflow on reccurences parameter
--FILE--
<?php
$start = new DateTime('2018-12-31 00:00:00');
$interval = new DateInterval('P1M');

try {
	new DatePeriod($start, $interval, 2147483640);
} catch (Exception $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
	new DatePeriod($start, $interval, 2147483639, DatePeriod::EXCLUDE_START_DATE | DatePeriod::INCLUDE_END_DATE);
} catch (Exception $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECTF--
DateMalformedPeriodStringException: DatePeriod::__construct(): Recurrence count must be greater or equal to 1 and lower than %d
DateMalformedStringException: DatePeriod::__construct(): Recurrence count must be greater or equal to 1 and lower than %d (including options)
