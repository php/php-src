--TEST--
IntlCalendar::toDateTime(): basic test
--SKIPIF--
<?php
if (!extension_loaded('intl'))
    die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
//ini_set("intl.default_locale", "nl");
ini_set('date.timezone', 'Europe/Lisbon');

$cal = new IntlGregorianCalendar(2012,04,17,17,35,36);

$dt = $cal->toDateTime();

var_dump($dt->format("c"), $dt->getTimeZone()->getName());
?>
--EXPECT--
string(25) "2012-05-17T17:35:36+01:00"
string(13) "Europe/Lisbon"
