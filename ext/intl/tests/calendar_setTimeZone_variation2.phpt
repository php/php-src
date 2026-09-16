--TEST--
IntlCalendar::setTimeZone(): different ways to specify time zone
--EXTENSIONS--
intl
--FILE--
<?php

$intlcal = new IntlGregorianCalendar();
$intlcal->setTimeZone('Europe/Paris');
var_dump($intlcal->getTimeZone()->getID());
$intlcal->setTimeZone(new DateTimeZone('Europe/Madrid'));
var_dump($intlcal->getTimeZone()->getID());

$pstdate = new DateTime('2012-01-01 00:00:00 PST');
$intlcal->setTimeZone($pstdate->getTimeZone());
var_dump($intlcal->getTimeZone()->getID());

$offsetdate = new DateTime('2012-01-01 00:00:00 -02:30');
$intlcal->setTimeZone($offsetdate->getTimeZone());
var_dump($intlcal->getTimeZone()->getID());
?>
--EXPECTF--
string(12) "Europe/Paris"
string(13) "Europe/Madrid"
string(3) "PST"
string(%d) "GMT-02%S30"
