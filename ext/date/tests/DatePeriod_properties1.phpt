--TEST--
DatePeriod: Test read only properties
--INI--
date.timezone=UTC
--FILE--
<?php

$start = new DateTime;
$interval = new DateInterval('P1D');
$end = new DateTime;
$period = new DatePeriod($start, $interval, $end);

echo "recurrences: ";
var_dump($period->recurrences);

echo "include_start_date: ";
var_dump($period->include_start_date);

echo "start: ";
var_dump($period->start == $start);

echo "current: ";
var_dump($period->current);

echo "end: ";
var_dump($period->end == $end);

echo "interval: ";
var_dump($period->interval->format("%R%d"));
?>
--EXPECT--
recurrences: int(1)
include_start_date: bool(true)
start: bool(true)
current: NULL
end: bool(true)
interval: string(2) "+1"
