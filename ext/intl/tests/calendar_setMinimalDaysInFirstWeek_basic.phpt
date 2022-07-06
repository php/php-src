--TEST--
IntlCalendar::setMinimalDaysInFirstWeek() basic test
--SKIPIF--
<?php
if (!extension_loaded('intl'))
    die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
ini_set("intl.default_locale", "nl");

$intlcal = IntlCalendar::createInstance('UTC');
var_dump(
        $intlcal->setMinimalDaysInFirstWeek(6),
        $intlcal->getMinimalDaysInFirstWeek(),
        intlcal_set_minimal_days_in_first_week($intlcal, 5),
        $intlcal->getMinimalDaysInFirstWeek()
);
?>
--EXPECT--
bool(true)
int(6)
bool(true)
int(5)
