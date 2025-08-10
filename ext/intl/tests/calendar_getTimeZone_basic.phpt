--TEST--
IntlCalendar::getTimeZone() basic test
--EXTENSIONS--
intl
--FILE--
<?php

$intlcal = IntlCalendar::createInstance('GMT+00:01');
print_r($intlcal->getTimeZone());
print_r(intlcal_get_time_zone($intlcal));
?>
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
