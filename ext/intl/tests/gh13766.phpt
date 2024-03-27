--TEST--
IntlDateFormatter::parse update its calendar
--EXTENSIONS--
intl
--FILE--
<?php
$oIntlDateFormatter = new IntlDateFormatter("en_GB");
$oIntlDateFormatter->setTimeZone('Europe/Berlin');
$oIntlDateFormatter->setPattern('VV');

var_dump($oIntlDateFormatter->parse('America/Los_Angeles', $offset1));
var_dump($oIntlDateFormatter->getTimeZone()->getID());
var_dump($oIntlDateFormatter->parseToCalendar('America/Los_Angeles', $offset2));
var_dump($oIntlDateFormatter->getTimeZone()->getID());
--EXPECTF--
int(%d)
string(13) "Europe/Berlin"
int(%d)
string(19) "America/Los_Angeles"
