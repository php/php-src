--TEST--
IntlTimeZone::getGMT(): basic test
--EXTENSIONS--
intl
--FILE--
<?php
$tz = IntlTimeZone::getGMT();
print_r($tz);
$tz = intltz_get_gmt();
print_r($tz);
?>
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
