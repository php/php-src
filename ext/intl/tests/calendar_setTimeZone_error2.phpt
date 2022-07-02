--TEST--
IntlCalendar::setTimeZone(): valid time zones for DateTime but not ICU
--EXTENSIONS--
intl
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
ini_set("intl.default_locale", "nl");
date_default_timezone_set('Europe/Amsterdam');

$intlcal = new IntlGregorianCalendar();

$pstdate = new DateTime('2012-01-01 00:00:00 WEST');
$intlcal->setTimeZone($pstdate->getTimeZone());
var_dump($intlcal->getTimeZone()->getID());

$pstdate = new DateTime('2012-01-01 00:00:00 +24:00');
$intlcal->setTimeZone($pstdate->getTimeZone());
var_dump($intlcal->getTimeZone()->getID());
?>
--EXPECTF--
Warning: IntlCalendar::setTimeZone(): intlcal_set_time_zone: time zone id 'WEST' extracted from ext/date DateTimeZone not recognized in %s on line %d
string(16) "Europe/Amsterdam"

Warning: IntlCalendar::setTimeZone(): intlcal_set_time_zone: object has an time zone offset that's too large in %s on line %d
string(16) "Europe/Amsterdam"
