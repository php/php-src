--TEST--
IntlCalendar::setTime() basic test
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

$time = strtotime('2012-02-29 00:00:00 +0000');

$intlcal = IntlCalendar::createInstance('UTC');
$intlcal->setTime($time * 1000);

var_dump(
	(float)$time*1000,
	$intlcal->getTime());
	
$intlcal = IntlCalendar::createInstance('UTC');
intlcal_set_time($intlcal,$time * 1000);
var_dump(intlcal_get_time($intlcal));

?>
==DONE==
--EXPECT--
float(1330473600000)
float(1330473600000)
float(1330473600000)
==DONE==