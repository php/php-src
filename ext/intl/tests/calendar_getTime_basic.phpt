--TEST--
IntlCalendar::getTime() basic test
--INI--
date.timezone=Atlantic/Azores
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
ini_set("intl.default_locale", "nl");

$intlcal = IntlCalendar::createInstance('UTC');
$intlcal->clear();
$intlcal->set(IntlCalendar::FIELD_YEAR, 2012);
$intlcal->set(IntlCalendar::FIELD_MONTH, 1 /* Feb */);
$intlcal->set(IntlCalendar::FIELD_DAY_OF_MONTH, 29);

$time = strtotime('2012-02-29 00:00:00 +0000');

var_dump((float)$time*1000, $intlcal->getTime());

?>
==DONE==
--EXPECT--
float(1330473600000)
float(1330473600000)
==DONE==