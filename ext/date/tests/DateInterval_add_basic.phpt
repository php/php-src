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

$interval1->invert = 1;
$interval = $interval->add($interval1);
echo $interval->format('%Y-%M-%D %H:%i:%s') . "\n";

$i0 = new Dateinterval('P1Y4D');
$i1 = new DateInterval('P1Y3D');
$i0->invert = 1;

$i2 = $i0->add($i1); /* -1 day */
echo $i2->format('%y-%m-%d %h-%i-%s') . "\n";
echo $i2->invert . "\n";

$i2 = $i0->sub($i1); /* -(2 years and 7 days) */
echo $i2->format('%y-%m-%d %h-%i-%s') . "\n";
echo $i2->invert . "\n";

$i0->invert = 0;
$i2 = $i0->sub($i1);
echo $i2->format('%y-%m-%d %h-%i-%s') . "\n";
echo $i2->invert . "\n";

$i0 = new Dateinterval('P1Y4DT3H');
$i1 = new DateInterval('P1Y4DT3H2M');
$i2 = $i0->sub($i1); /* -2 minutes */
echo $i2->format('%y-%m-%d %h-%i-%s') . "\n";
echo $i2->invert . "\n";

$i2 = $i1->sub($i0);
echo $i2->format('%y-%m-%d %h-%i-%s') . "\n";
echo $i2->invert . "\n";

?>
--EXPECT--
00-00-09 22:30:23
01-02-09 03:40:50
01-02-19 02:11:13
01-02-09 03:40:50
0-0-1 0-0-0
1
2-0-7 0-0-0
1
0-0-1 0-0-0
0
0-0-0 0-2-0
1
0-0-0 0-2-0
0
