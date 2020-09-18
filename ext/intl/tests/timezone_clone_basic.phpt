--TEST--
IntlTimeZone clone handler: basic test
--SKIPIF--
<?php
if (!extension_loaded('intl'))
    die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

$tz1 = IntlTimeZone::createTimeZone('Europe/Amsterdam');
print_r($tz1);
print_r(clone $tz1);

//clone non-owned object
$gmt = IntlTimeZone::getGMT();
print_r($gmt);
print_r(clone $gmt);

?>
--EXPECTF--
IntlTimeZone Object
(
    [valid] => 1
    [id] => Europe/Amsterdam
    [rawOffset] => 3600000
    [currentOffset] => %d
)
IntlTimeZone Object
(
    [valid] => 1
    [id] => Europe/Amsterdam
    [rawOffset] => 3600000
    [currentOffset] => %d
)
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