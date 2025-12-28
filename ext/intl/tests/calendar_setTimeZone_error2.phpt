--TEST--
IntlCalendar::setTimeZone(): valid time zones for DateTime but not ICU
--EXTENSIONS--
intl
--INI--
date.timezone=Europe/Amsterdam
intl.default_locale=nl
--FILE--
<?php

$intlcal = new IntlGregorianCalendar();

$pstdate = new DateTime('2012-01-01 00:00:00 WEST');
var_dump($intlcal->setTimeZone($pstdate->getTimeZone()));
var_dump($intlcal->getErrorMessage());
var_dump($intlcal->getTimeZone()->getID());

$pstdate = new DateTime('2012-01-01 00:00:00 +24:00');
var_dump($intlcal->setTimeZone($pstdate->getTimeZone()));
var_dump($intlcal->getErrorMessage());
var_dump($intlcal->getTimeZone()->getID());
?>
--EXPECT--
bool(false)
string(126) "IntlCalendar::setTimeZone(): time zone id 'WEST' extracted from ext/date DateTimeZone not recognized: U_ILLEGAL_ARGUMENT_ERROR"
string(16) "Europe/Amsterdam"
bool(false)
string(102) "IntlCalendar::setTimeZone(): object has an time zone offset that's too large: U_ILLEGAL_ARGUMENT_ERROR"
string(16) "Europe/Amsterdam"
