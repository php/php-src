--TEST--
Test for bug #67634: getTimestamp() modify instance of DateTimeImmutable
--CREDITS--
Boro Sitnikovski <buritomath@yahoo.com>
--INI--
date.timezone = UTC
--FILE--
<?php
$date = new DateTimeImmutable('2014-05');

$date2 = $date->modify('first day of next month midnight - 1 second');

var_dump($date2->format('c'));
var_dump($date2->format('U'));
var_dump($date2->format('c'));
var_dump($date2->getTimestamp());
var_dump($date2->format('U'));
var_dump($date2->format('c'));
?>
--EXPECT--
string(25) "2014-05-31T23:59:59+00:00"
string(10) "1401580799"
string(25) "2014-05-31T23:59:59+00:00"
int(1398988799)
string(10) "1401580799"
string(25) "2014-05-31T23:59:59+00:00"
