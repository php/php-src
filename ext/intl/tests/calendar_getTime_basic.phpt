--TEST--
IntlCalendar::getTime() basic test
--EXTENSIONS--
intl
--FILE--
<?php

$intlcal = IntlCalendar::createInstance('UTC');
$intlcal->clear();
$intlcal->set(IntlCalendar::FIELD_YEAR, 2012);
$intlcal->set(IntlCalendar::FIELD_MONTH, 1 /* Feb */);
$intlcal->set(IntlCalendar::FIELD_DAY_OF_MONTH, 29);

$time = strtotime('2012-02-29 00:00:00 +0000');

var_dump((float)$time*1000, $intlcal->getTime());

?>
--EXPECT--
float(1330473600000)
float(1330473600000)
