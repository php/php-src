--TEST--
IntlCalendar::setTime() rejects non-finite timestamps
--CREDITS--
Wong Wai Kin
--EXTENSIONS--
intl
--FILE--
<?php
$calendar = IntlCalendar::createInstance('UTC');
var_dump($calendar->setTime(1234));

foreach ([NAN, INF, -INF] as $timestamp) {
    var_dump($calendar->setTime($timestamp));
    var_dump($calendar->getErrorCode() === U_ILLEGAL_ARGUMENT_ERROR);
    var_dump($calendar->getTime());
}
?>
--EXPECT--
bool(true)
bool(false)
bool(true)
float(1234)
bool(false)
bool(true)
float(1234)
bool(false)
bool(true)
float(1234)
