--TEST--
IntlTimeZone::useDaylightTime: basic test
--SKIPIF--
<?php
if (!extension_loaded('intl'))
    die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
$lsb = IntlTimeZone::createTimeZone('Europe/Lisbon');
$gmt = IntlTimeZone::getGMT();

var_dump($lsb->useDaylightTime());
var_dump($gmt->useDaylightTime());

var_dump(intltz_use_daylight_time($lsb));
var_dump(intltz_use_daylight_time($gmt));
?>
--EXPECT--
bool(true)
bool(false)
bool(true)
bool(false)