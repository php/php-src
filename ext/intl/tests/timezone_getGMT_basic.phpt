--TEST--
IntlTimeZone::getGMT(): basic test
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
$tz = IntlTimeZone::getGMT();
print_r($tz);
$tz = intltz_get_gmt();
print_r($tz);
?>
==DONE==
--EXPECT--
IntlTimeZone Object
(
    [valid] => 1
    [id] => GMT
    [rawOffset] => 0
    [currentOffset] => 0
)
IntlTimeZone Object
(
    [valid] => 1
    [id] => GMT
    [rawOffset] => 0
    [currentOffset] => 0
)
==DONE==