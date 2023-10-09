--TEST--
Test IntlCalendar::setDateTime()
--EXTENSIONS--
intl
--FILE--
<?php

$intlcal = IntlCalendar::createInstance('UTC');
$intlcal->clear();

//two minutes to midnight!
$intlcal->setDateTime(2012, 1, 29, 23, 58);
var_dump($intlcal->getTime(), strtotime('2012-02-29 23:58:00 +0000') * 1000.);

$intlcal->setDateTime(2012, 1, 29, 23, 58, 31);
var_dump($intlcal->getTime(), strtotime('2012-02-29 23:58:31 +0000') * 1000.);

?>
--EXPECT--
float(1330559880000)
float(1330559880000)
float(1330559911000)
float(1330559911000)
