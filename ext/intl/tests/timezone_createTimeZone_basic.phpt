--TEST--
IntlTimeZone::createTimeZone(): basic test
--EXTENSIONS--
intl
--FILE--
<?php
$tz = IntlTimeZone::createTimeZone('GMT+01:00');
print_r($tz);
$tz = intltz_create_time_zone('GMT+01:00');
print_r($tz);
?>
--EXPECT--
IntlTimeZone Object
(
    [valid] => 1
    [id] => GMT+01:00
    [rawOffset] => 3600000
    [currentOffset] => 3600000
)
IntlTimeZone Object
(
    [valid] => 1
    [id] => GMT+01:00
    [rawOffset] => 3600000
    [currentOffset] => 3600000
)
