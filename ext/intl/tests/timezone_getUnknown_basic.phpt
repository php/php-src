--TEST--
IntlCalendar::getUnknown(): basic test
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
if (version_compare(INTL_ICU_VERSION, '49') < 0)
	die('skip for ICU 49+');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
ini_set("intl.default_locale", "nl");

$tz = IntlTimeZone::getUnknown();
print_r($tz);
$tz = intltz_get_unknown();
print_r($tz);
?>
==DONE==
--EXPECT--
IntlTimeZone Object
(
    [valid] => 1
    [id] => Etc/Unknown
    [rawOffset] => 0
    [currentOffset] => 0
)
IntlTimeZone Object
(
    [valid] => 1
    [id] => Etc/Unknown
    [rawOffset] => 0
    [currentOffset] => 0
)
==DONE==
