--TEST--
Bug GH-14709 overflow on reccurences parameter
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only");
?>
--FILE--
<?php
$start = new DateTime('2018-12-31 00:00:00');
$interval = new DateInterval('P1M');

try {
	new DatePeriod($start, $interval, PHP_INT_MAX);
} catch (Exception $e) {
	echo $e->getMessage();
}
?>
--EXPECTF--
DatePeriod::__construct(): Recurrence count must be between 1 and %d
