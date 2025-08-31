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
$offset3 = "offset";

try {
	$oIntlDateFormatter->parseToCalendar('America/Los_Angeles', $offset3);
} catch (\TypeError $e) {
	echo $e->getMessage() . PHP_EOL;
}
$offset3 = PHP_INT_MAX * 16;
try {
	$oIntlDateFormatter->parseToCalendar('America/Los_Angeles', $offset3);
} catch (\ValueError $e) {
	echo $e->getMessage();
}
--EXPECTF--
int(%d)
string(13) "Europe/Berlin"
int(%d)
string(19) "America/Los_Angeles"
IntlDateFormatter::parseToCalendar(): Argument #2 ($offset) must be of type int, string given

Deprecated: Implicit conversion from float %r(1\.4757395258967641E\+20|34359738352)%r to int loses precision in %s on line %d
