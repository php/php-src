--TEST--
IntlGregorianCalendar::__construct(): basic
--SKIPIF--
<?php
if (!extension_loaded('intl'))
    die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
ini_set("intl.default_locale", "nl");

date_default_timezone_set('Europe/Amsterdam');

$intlcal = intlgregcal_create_instance();
var_dump($intlcal->getTimeZone()->getId());
var_dump($intlcal->getLocale(1));

$intlcal = new IntlGregorianCalendar('Europe/Lisbon', NULL);
var_dump($intlcal->getTimeZone()->getId());
var_dump($intlcal->getLocale(1));

$intlcal = new IntlGregorianCalendar(NULL, 'pt_PT');
var_dump($intlcal->getTimeZone()->getId());
var_dump($intlcal->getLocale(1));

$intlcal = new IntlGregorianCalendar('Europe/Lisbon', 'pt_PT');
var_dump($intlcal->getTimeZone()->getId());
var_dump($intlcal->getLocale(1));

$intlcal = new IntlGregorianCalendar('Europe/Paris', 'fr_CA', NULL, NULL, NULL, NULL);
var_dump($intlcal->getTimeZone()->getId());
var_dump($intlcal->getLocale(1));

var_dump($intlcal->getType());
?>
--EXPECT--
string(16) "Europe/Amsterdam"
string(5) "nl_NL"
string(13) "Europe/Lisbon"
string(5) "nl_NL"
string(16) "Europe/Amsterdam"
string(5) "pt_PT"
string(13) "Europe/Lisbon"
string(5) "pt_PT"
string(12) "Europe/Paris"
string(5) "fr_CA"
string(9) "gregorian"
