--TEST--
IntlDateFormatter, calendars and time zone
--INI--
date.timezone=Atlantic/Azores
--SKIPIF--
<?php
if (!extension_loaded('intl'))	die('skip intl extension not enabled'); ?>
<?php if (version_compare(INTL_ICU_VERSION, '50.1.2') >=  0) die('skip for ICU < 50.1.2'); ?>
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

$fmt1 = new IntlDateFormatter('en_US',
	IntlDateFormatter::FULL,
	IntlDateFormatter::FULL,
	'GMT+05:12',
	IntlDateFormatter::TRADITIONAL);
$fmt2 = new IntlDateFormatter('en_US',
	IntlDateFormatter::FULL,
	IntlDateFormatter::FULL,
	'GMT+05:12',
	IntlDateFormatter::GREGORIAN);
$fmt3 = new IntlDateFormatter('en_US@calendar=hebrew',
	IntlDateFormatter::FULL,
	IntlDateFormatter::FULL,
	'GMT+05:12',
	IntlDateFormatter::TRADITIONAL);
var_dump($fmt1->format(strtotime('2012-01-01 00:00:00 +0000')));
var_dump($fmt2->format(strtotime('2012-01-01 00:00:00 +0000')));
var_dump($fmt3->format(strtotime('2012-01-01 00:00:00 +0000')));

new IntlDateFormatter('en_US@calendar=hebrew',
	IntlDateFormatter::FULL,
	IntlDateFormatter::FULL,
	'GMT+05:12',
	-1);
?>
==DONE==
--EXPECTF--
string(44) "Sunday, January 1, 2012 5:12:00 AM GMT+05:12"
string(44) "Sunday, January 1, 2012 5:12:00 AM GMT+05:12"
string(42) "Sunday, Tevet 6, 5772 5:12:00 AM GMT+05:12"

Warning: IntlDateFormatter::__construct(): datefmt_create: invalid value for calendar type; it must be one of IntlDateFormatter::TRADITIONAL (locale's default calendar) or IntlDateFormatter::GREGORIAN. Alternatively, it can be an IntlCalendar object in %s on line %d
==DONE==
