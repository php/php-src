--TEST--
date_create() function [1]
--SKIPIF--
<?php if (!function_exists('date_create')) echo "SKIP"; ?>
--FILE--
<?php
date_default_timezone_set('Europe/Oslo');
$tz1 = timezone_open("GMT");
$tz2 = timezone_open("Europe/London");
$tz3 = timezone_open("America/Los_Angeles");
$d = array();
$d[] = date_create("2005-07-14 22:30:41");
$d[] = date_create("2005-07-14 22:30:41 GMT");
$d[] = date_create("2005-07-14 22:30:41 CET");
$d[] = date_create("2005-07-14 22:30:41 CEST");
$d[] = date_create("2005-07-14 22:30:41 Europe/Oslo");
$d[] = date_create("2005-07-14 22:30:41 America/Los_Angeles");

$d[] = date_create("2005-07-14 22:30:41", $tz1);
$d[] = date_create("2005-07-14 22:30:41", $tz2);
$d[] = date_create("2005-07-14 22:30:41", $tz3);

$d[] = date_create("2005-07-14 22:30:41 GMT", $tz1);
$d[] = date_create("2005-07-14 22:30:41 GMT", $tz2);
$d[] = date_create("2005-07-14 22:30:41 GMT", $tz3);

$d[] = date_create("2005-07-14 22:30:41 Europe/Oslo", $tz1);
$d[] = date_create("2005-07-14 22:30:41 America/Los_Angeles", $tz2);

foreach($d as $date) {
	echo $date->format(DateTime::ISO8601), "\n";
}
?>
--EXPECT--
2005-07-14T22:30:41+0200
2005-07-14T22:30:41+0000
2005-07-14T22:30:41+0100
2005-07-14T22:30:41+0200
2005-07-14T22:30:41+0200
2005-07-14T22:30:41-0700
2005-07-14T22:30:41+0000
2005-07-14T22:30:41+0100
2005-07-14T22:30:41-0700
2005-07-14T22:30:41+0000
2005-07-14T22:30:41+0000
2005-07-14T22:30:41+0000
2005-07-14T22:30:41+0200
2005-07-14T22:30:41-0700
