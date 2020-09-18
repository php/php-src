--TEST--
IntlCalendar::isLenient(), ::setLenient() basic test
--SKIPIF--
<?php
if (!extension_loaded('intl'))
    die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
ini_set("intl.default_locale", "nl");

$intlcal1 = IntlCalendar::createInstance('UTC');
var_dump($intlcal1->isLenient());
var_dump(intlcal_is_lenient($intlcal1));
var_dump($intlcal1->setLenient(false));
var_dump($intlcal1->isLenient());
var_dump(intlcal_set_lenient($intlcal1, true));
var_dump($intlcal1->isLenient());
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(false)
bool(true)
bool(true)