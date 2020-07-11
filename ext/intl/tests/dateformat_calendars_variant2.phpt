--TEST--
IntlDateFormatter, calendars and time zone
--INI--
date.timezone=Atlantic/Azores
--SKIPIF--
<?php
if (!extension_loaded('intl'))	die('skip intl extension not enabled'); ?>
<?php if (version_compare(INTL_ICU_VERSION, '51.2') < 0) die('skip for ICU >= 51.2'); ?>
<?php if (version_compare(INTL_ICU_VERSION, '54.1') >=  0) die('skip for ICU < 54.1'); ?>
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
string(47) "Sunday, January 1, 2012 at 5:12:00 AM GMT+05:12"
string(47) "Sunday, January 1, 2012 at 5:12:00 AM GMT+05:12"
string(48) "Sunday, Tevet 6, 5772 AM at 5:12:00 AM GMT+05:12"

Fatal error: Uncaught IntlException: IntlDateFormatter::__construct(): datefmt_create: Invalid value for calendar type; it must be one of IntlDateFormatter::TRADITIONAL (locale's default calendar) or IntlDateFormatter::GREGORIAN. Alternatively, it can be an IntlCalendar object in %sdateformat_calendars_variant2.php:27
Stack trace:
#0 %sdateformat_calendars_variant2.php(27): IntlDateFormatter->__construct('en_US@calendar=...', 0, 0, 'GMT+05:12', -1)
#1 {main}
  thrown in %sdateformat_calendars_variant2.php on line 27
