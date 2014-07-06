--TEST--
IntlDateFormatter: setTimeZoneID() deprecation
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
ini_set("intl.default_locale", "pt_PT");
ini_set("date.timezone", 'Atlantic/Azores');

$df = new IntlDateFormatter('pt_PT', 0, 0, 'Europe/Minsk');

$df->setTimeZoneId('Europe/Madrid');

?>
==DONE==
--EXPECTF--

Deprecated: IntlDateFormatter::setTimeZoneId(): Use datefmt_set_timezone() instead, which also accepts a plain time zone identifier and for which this function is now an alias in %s on line %d
==DONE==
