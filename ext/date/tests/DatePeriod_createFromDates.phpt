--TEST--
Test DatePeriod::createFromDates()
--FILE--
<?php

class MyDatePeriod extends DatePeriod
{
}

$start = new DateTime();
$end = new DateTime("+2 days");
$interval = new DateInterval("P1D");

$p1 = MyDatePeriod::createFromDates($start, $interval, $end);
$p2 = new MyDatePeriod($start, $interval, $end);

var_dump($p1::class);
var_dump($p1 == $p2);

?>
--EXPECT--
string(12) "MyDatePeriod"
bool(true)
