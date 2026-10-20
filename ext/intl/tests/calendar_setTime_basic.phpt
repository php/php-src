--TEST--
IntlCalendar::setTime() basic test
--EXTENSIONS--
intl
--FILE--
<?php

$time = strtotime('2012-02-29 00:00:00 +0000');

$intlcal = IntlCalendar::createInstance('UTC');
$intlcal->setTime($time * 1000);

var_dump(
    (float)$time*1000,
    $intlcal->getTime());

$intlcal = IntlCalendar::createInstance('UTC');
intlcal_set_time($intlcal,$time * 1000);
var_dump(intlcal_get_time($intlcal));

?>
--EXPECT--
float(1330473600000)
float(1330473600000)
float(1330473600000)
