--TEST--
DatePeriod: Test getter
--INI--
date.timezone=UTC
--FILE--
<?php
$start = new DateTime('2000-01-01 00:00:00', new DateTimeZone('Europe/Berlin'));
$end   = new DateTime('2000-01-31 00:00:00', new DateTimeZone('UTC'));
$interval = new DateInterval('P1D');
$period   = new DatePeriod($start, $interval, $end);
$recurrences = 4;

var_dump($period->getStartDate()->format('Y-m-d H:i:s'));
var_dump($period->getStartDate()->getTimeZone()->getName());

var_dump($period->getEndDate()->format('Y-m-d H:i:s'));
var_dump($period->getEndDate()->getTimeZone()->getName());

var_dump($period->getDateInterval()->format('%R%y-%m-%d-%h-%i-%s'));
var_dump($period->getRecurrences());

$periodWithRecurrences = new DatePeriod($start, $interval, $recurrences);

var_dump($periodWithRecurrences->getRecurrences());
var_dump($periodWithRecurrences->getEndDate());

$periodWithRecurrencesWithoutStart = new DatePeriod($start, $interval, $recurrences, DatePeriod::EXCLUDE_START_DATE);

var_dump($periodWithRecurrences->getRecurrences());

?>
--EXPECT--
string(19) "2000-01-01 00:00:00"
string(13) "Europe/Berlin"
string(19) "2000-01-31 00:00:00"
string(3) "UTC"
string(12) "+0-0-1-0-0-0"
NULL
int(4)
NULL
int(4)
