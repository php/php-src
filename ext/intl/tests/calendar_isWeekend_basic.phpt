--TEST--
IntlCalendar::isWeekend basic test
--INI--
date.timezone=Atlantic/Azores
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
if (version_compare(INTL_ICU_VERSION, '4.4') < 0)
	die('skip for ICU 4.4+');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
ini_set("intl.default_locale", "nl");

$intlcal = IntlCalendar::createInstance('UTC');
var_dump($intlcal->isWeekend(strtotime('2012-02-29 12:00:00 +0000') * 1000));
var_dump(intlcal_is_weekend($intlcal, strtotime('2012-02-29 12:00:00 +0000') * 1000));
var_dump($intlcal->isWeekend(strtotime('2012-03-11 12:00:00 +0000') * 1000));
?>
==DONE==
--EXPECT--
bool(false)
bool(false)
bool(true)
==DONE==