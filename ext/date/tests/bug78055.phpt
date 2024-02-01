--TEST--
Bug #78055 (DatePeriod's getRecurrences and ->recurrences don't match)
--FILE--
<?php
$start = new DateTime('2018-12-31 00:00:00');
$end   = new DateTime('2021-12-31 00:00:00');
$interval = new DateInterval('P1M');
$recurrences = 5;

$period = new DatePeriod($start, $interval, $recurrences, DatePeriod::EXCLUDE_START_DATE);
echo $period->getRecurrences(), " ", $period->recurrences, "\n";

$period = new DatePeriod($start, $interval, $recurrences);
echo $period->getRecurrences(), " ", $period->recurrences, "\n";

$period = new DatePeriod($start, $interval, $recurrences, DatePeriod::INCLUDE_END_DATE);
echo $period->getRecurrences(), " ", $period->recurrences, "\n";

$period = new DatePeriod($start, $interval, $end);
echo $period->getRecurrences(), " ", $period->recurrences, "\n";

$period = new DatePeriod($start, $interval, $end, DatePeriod::EXCLUDE_START_DATE);
echo $period->getRecurrences(), " ", $period->recurrences, "\n";
?>
--EXPECT--
5 5
5 6
5 7
 1
 0
