--TEST--
IntlGregorianCalendar::__construct(): argument variants
--SKIPIF--
<?php
if (!extension_loaded('intl'))
    die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

date_default_timezone_set('Europe/Amsterdam');

$intlcal = intlgregcal_create_instance(2012, 1, 29, 16, 0, NULL);
var_dump($intlcal->getTimeZone()->getId());
var_dump($intlcal->getTime(), (float)strtotime('2012-02-29 16:00:00') * 1000);

$intlcal = new IntlGregorianCalendar(2012, 1, 29, 16, 7, 8);
var_dump($intlcal->getTime(), (float)strtotime('2012-02-29 16:07:08') * 1000);

var_dump($intlcal->getType());
?>
--EXPECT--
string(16) "Europe/Amsterdam"
float(1330527600000)
float(1330527600000)
float(1330528028000)
float(1330528028000)
string(9) "gregorian"
