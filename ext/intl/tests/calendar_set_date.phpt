--TEST--
Test IntlCalendar::setDate()
--EXTENSIONS--
intl
--FILE--
<?php

$intlcal = IntlCalendar::createInstance('UTC');
$intlcal->clear();

$intlcal->setDate(2012, 1, 29);
var_dump($intlcal->getTime(), strtotime('2012-02-29 00:00:00 +0000') * 1000.);

?>
--EXPECT--
float(1330473600000)
float(1330473600000)
