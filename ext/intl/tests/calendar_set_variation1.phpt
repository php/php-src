--TEST--
IntlCalendar::set() argument variations
--INI--
date.timezone=Atlantic/Azores
--EXTENSIONS--
intl
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
ini_set("intl.default_locale", "nl");

$intlcal = IntlCalendar::createInstance('UTC');
$intlcal->clear();
var_dump($intlcal->set(2012, 1, 29));
var_dump($intlcal->getTime(),
        strtotime('2012-02-29 00:00:00 +0000') * 1000.);

//two minutes to midnight!
var_dump($intlcal->set(2012, 1, 29, 23, 58));
var_dump($intlcal->getTime(),
        strtotime('2012-02-29 23:58:00 +0000') * 1000.);

var_dump($intlcal->set(2012, 1, 29, 23, 58, 31));
var_dump($intlcal->getTime(),
        strtotime('2012-02-29 23:58:31 +0000') * 1000.);

?>
--EXPECTF--
Deprecated: Calling IntlCalendar::set() with more than 2 arguments is deprecated, use either IntlCalendar::setDate() or IntlCalendar::setDateTime() instead in %s on line %d
bool(true)
float(1330473600000)
float(1330473600000)

Deprecated: Calling IntlCalendar::set() with more than 2 arguments is deprecated, use either IntlCalendar::setDate() or IntlCalendar::setDateTime() instead in %s on line %d
bool(true)
float(1330559880000)
float(1330559880000)

Deprecated: Calling IntlCalendar::set() with more than 2 arguments is deprecated, use either IntlCalendar::setDate() or IntlCalendar::setDateTime() instead in %s on line %d
bool(true)
float(1330559911000)
float(1330559911000)
