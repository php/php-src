--TEST--
IntlCalendar::getTimeZone() basic test
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
if (version_compare(INTL_ICU_VERSION, '4.8') < 0)
	die('skip for ICU 4.8+');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
ini_set("intl.default_locale", "nl");

$intlcal = IntlCalendar::createInstance('GMT+00:01');
print_r($intlcal->getTimeZone());
print_r(intlcal_get_time_zone($intlcal));
?>
==DONE==
--EXPECT--
IntlTimeZone Object
(
    [valid] => 1
    [id] => GMT+00:01
    [rawOffset] => 60000
    [currentOffset] => 60000
)
IntlTimeZone Object
(
    [valid] => 1
    [id] => GMT+00:01
    [rawOffset] => 60000
    [currentOffset] => 60000
)
==DONE==