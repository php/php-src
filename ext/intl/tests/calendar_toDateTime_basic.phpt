--TEST--
IntlCalendar::toDateTime(): basic test
--EXTENSIONS--
intl
--INI--
date.timezone=Europe/Lisbon
--FILE--
<?php

$cal = new IntlGregorianCalendar(2012,04,17,17,35,36);

$dt = $cal->toDateTime();

var_dump($dt->format("c"), $dt->getTimeZone()->getName());
?>
--EXPECTF--
Deprecated: Calling IntlGregorianCalendar::__construct() with more than 2 arguments is deprecated, use either IntlGregorianCalendar::createFromDate() or IntlGregorianCalendar::createFromDateTime() instead in %s on line %d
string(25) "2012-05-17T17:35:36+01:00"
string(13) "Europe/Lisbon"
