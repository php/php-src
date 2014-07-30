--TEST--
Test for bug #65672: Broken classes inherited from DatePeriod
--CREDITS--
Boro Sitnikovski <buritomath@yahoo.com>
--INI--
date.timezone = UTC 
--FILE--
<?php
class Period extends DatePeriod {
    public $test;
}

$p = new Period(new DateTime('now'), new DateInterval('P1Y'), new DateTime('tomorrow'));

$old_recurrence = $p->recurrences;
$old_test = $p->test;

$p->recurrences = $old_recurrence + 3;
$p->test = 123;

var_dump($p->recurrences == $old_recurrence);
var_dump($p->test != $old_test);
var_dump($p->test == 123);

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
