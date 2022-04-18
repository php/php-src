--TEST--
Test for bug #81458: Regression in PHP 8.1: Incorrect difference after timezone change
--FILE--
<?php
$first = (new DateTime('2018-07-01 00:00:00.000000 America/Toronto'))->setTimezone(new DateTimeZone('UTC'));
$second = new DateTime('2018-07-02 00:00:00.000000 America/Toronto');

var_dump($first->diff($second)->days);
var_dump($first->diff($second)->d);

date_default_timezone_set('UTC');
$a = new DateTime('2018-12-01 00:00');
$b = new DateTime('2018-12-02 00:01');

var_dump($a->diff($b)->days);
?>
--EXPECT--
int(1)
int(1)
int(1)
