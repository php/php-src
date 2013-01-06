--TEST--
IntlCalendar::setTimeZone() variation with NULL arg
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
ini_set("intl.default_locale", "nl");

$intlcal = IntlCalendar::createInstance('Europe/Amsterdam');
print_r($intlcal->getTimeZone()->getID());
echo "\n";
var_dump($intlcal->get(IntlCalendar::FIELD_ZONE_OFFSET));

/* passing NULL has no effect */
$intlcal->setTimeZone(null);
print_r($intlcal->getTimeZone()->getID());
echo "\n";
var_dump($intlcal->get(IntlCalendar::FIELD_ZONE_OFFSET));

?>
==DONE==
--EXPECT--
Europe/Amsterdam
int(3600000)
Europe/Amsterdam
int(3600000)
==DONE==