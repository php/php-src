--TEST--
IntlCalendar::clear() basic test
--SKIPIF--
<?php
if (!extension_loaded('intl'))
    die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
ini_set("intl.default_locale", "nl");

$intlcal = IntlCalendar::createInstance('UTC');
var_dump($intlcal->clear());
var_dump(
    $intlcal->get(IntlCalendar::FIELD_YEAR),
    $intlcal->get(IntlCalendar::FIELD_MONTH),
    $intlcal->get(IntlCalendar::FIELD_DAY_OF_MONTH),
    $intlcal->get(IntlCalendar::FIELD_HOUR),
    $intlcal->get(IntlCalendar::FIELD_MINUTE),
    $intlcal->get(IntlCalendar::FIELD_SECOND),
    $intlcal->get(IntlCalendar::FIELD_MILLISECOND)
);

$intlcal2 = IntlCalendar::createInstance('Europe/Amsterdam');
intlcal_clear($intlcal2, null);
var_dump($intlcal2->getTime());

?>
--EXPECT--
bool(true)
int(1970)
int(0)
int(1)
int(0)
int(0)
int(0)
int(0)
float(-3600000)