--TEST--
IntlGregorianCalendar::get/setGregorianChange(): basic test
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
ini_set("intl.default_locale", "nl");

date_default_timezone_set('Europe/Amsterdam');

$intlcal = new IntlGregorianCalendar();

var_dump($intlcal->getGregorianChange());

var_dump($intlcal->setGregorianChange(0));
var_dump(intlgregcal_get_gregorian_change($intlcal));

var_dump(intlgregcal_set_gregorian_change($intlcal, 1));
var_dump($intlcal->getGregorianChange());

?>
==DONE==
--EXPECT--
float(-12219292800000)
bool(true)
float(0)
bool(true)
float(1)
==DONE==