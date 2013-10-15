--TEST--
IntlTimeZone::getRawOffset(): basic test
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
$ams = IntlTimeZone::createTimeZone('Europe/Amsterdam');
var_dump($ams->getRawOffset());

$lsb = IntlTimeZone::createTimeZone('Europe/Lisbon');
var_dump(intltz_get_raw_offset($lsb));

?>
==DONE==
--EXPECT--
int(3600000)
int(0)
==DONE==