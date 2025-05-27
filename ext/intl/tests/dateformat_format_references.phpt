--TEST--
Fix dateformat_format() with array argument with values as references.
--EXTENSIONS--
intl
--SKIPIF--
<?php
if (PHP_OS_FAMILY === "Windows") die("skip currently unsupported on Windows");
?>
--FILE--
<?php
$a = 24;
$localtime_arr = array (
	'tm_sec' => &$a ,
	'tm_min' => 3,
	'tm_hour' => 19,
	'tm_mday' => 3,
	'tm_mon' => 3,
	'tm_year' => 105,
);
$fmt = datefmt_create('en_US', IntlDateFormatter::FULL, IntlDateFormatter::FULL, 'America/New_York', IntlDateFormatter::GREGORIAN);
$formatted = datefmt_format($fmt , $localtime_arr);
var_dump($formatted);
?>
--EXPECTF--
string(%d) "Sunday, April 3, 2005 at 7:03:24%aPM Eastern Daylight Time"
