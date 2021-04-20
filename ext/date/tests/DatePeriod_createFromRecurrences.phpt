--TEST--
Test DatePeriod::createFromRecurrences()
--FILE--
<?php

class MyDatePeriod extends DatePeriod
{
}

$start = new DateTime();
$interval = new DateInterval("P1D");

$p1 = MyDatePeriod::createFromRecurrences($start, $interval, 2);
$p2 = new MyDatePeriod($start, $interval, 2);

var_dump($p1::class);
var_dump($p1 == $p2);

try {
    MyDatePeriod::createFromRecurrences($start, $interval, 0);
} catch (Exception $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECT--
string(12) "MyDatePeriod"
bool(true)
DatePeriod::createFromRecurrences(): Recurrence count must be greater than 0
