--TEST--
Test DateInterval::add() basic functionality
--CREDITS--
Jakub Zelenka <bukka@php.net>
--SKIPIF--
<?php if (!method_exists('DateInterval', 'add')) die("skip: method doesn't exist"); ?>
--FILE--
<?php
date_default_timezone_set('UTC');

$date11 = new DateTime('2000-01-01 00:00:00');
$date12 = new DateTime('2000-01-10 22:30:23');
$interval1 = $date11->diff($date12);
echo $interval1->format('%Y-%M-%D %H:%i:%s') . "\n";

$date21 = new DateTime('2000-01-01 00:00:00');
$date22 = new DateTime('2001-03-10 03:40:50');
$interval2 = $date21->diff($date22);
echo $interval2->format('%Y-%M-%D %H:%i:%s') . "\n";

$interval = $interval1->add($interval2);
echo $interval->format('%Y-%M-%D %H:%i:%s') . "\n";

?>
--EXPECT--
00-00-09 22:30:23
01-02-09 03:40:50
01-02-19 02:11:13
