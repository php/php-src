--TEST--
IntlCalendar::set() argument variations
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
var_dump($intlcal->set(2012, 1, 29));
var_dump($intlcal->getTime(),
		strtotime('2012-02-29 00:00:00 +0000') * 1000.);

//two minutes to midnight!
var_dump($intlcal->set(2012, 1, 29, 23, 58));
var_dump($intlcal->getTime(),
		strtotime('2012-02-29 23:58:00 +0000') * 1000.);

var_dump($intlcal->set(2012, 1, 29, 23, 58, 31));
var_dump($intlcal->getTime(),
		strtotime('2012-02-29 23:58:31 +0000') * 1000.);

?>
==DONE==
--EXPECT--
bool(true)
float(1330473600000)
float(1330473600000)
bool(true)
float(1330559880000)
float(1330559880000)
bool(true)
float(1330559911000)
float(1330559911000)
==DONE==