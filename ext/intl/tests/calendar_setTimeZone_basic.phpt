--TEST--
IntlCalendar::setTimeZone() basic test
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

$intlcal->setTimeZone(IntlTimeZone::getGMT());
print_r($intlcal->getTimeZone()->getID());
echo "\n";
var_dump($intlcal->get(IntlCalendar::FIELD_ZONE_OFFSET));

intlcal_set_time_zone($intlcal,
        IntlTimeZone::createTimeZone('GMT+05:30'));
print_r($intlcal->getTimeZone()->getID());
echo "\n";
var_dump($intlcal->get(IntlCalendar::FIELD_ZONE_OFFSET));

?>
--EXPECT--
Europe/Amsterdam
int(3600000)
GMT
int(0)
GMT+05:30
int(19800000)
