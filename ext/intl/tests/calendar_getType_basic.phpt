--TEST--
IntlCalendar::getType() basic test
--INI--
date.timezone=Atlantic/Azores
--SKIPIF--
<?php
if (!extension_loaded('intl'))
    die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
ini_set("intl.default_locale", "nl");

$intlcal = IntlCalendar::createInstance();
VAR_DUMP($intlcal->getType());
$intlcal = IntlCalendar::createInstance(null, "nl_NL@calendar=hebrew");
VAR_DUMP(intlcal_get_type($intlcal));
?>
--EXPECT--
string(9) "gregorian"
string(6) "hebrew"