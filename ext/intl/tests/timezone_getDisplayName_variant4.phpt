--TEST--
IntlTimeZone::getDisplayName(): type parameter (ICU >= 51.2)
--SKIPIF--
<?php if (!extension_loaded('intl')) die('skip intl extension not enabled'); ?>
<?php if (version_compare(INTL_ICU_VERSION, '51.2') < 0) die('skip for ICU >= 51.2'); ?>
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
ini_set("error_reporting", -1);
ini_set("display_errors", 1);

$lsb = IntlTimeZone::createTimeZone('Europe/Lisbon');

ini_set('intl.default_locale', 'en_US');
var_dump($lsb->getDisplayName(false, IntlTimeZone::DISPLAY_SHORT));
var_dump($lsb->getDisplayName(false, IntlTimeZone::DISPLAY_LONG));
var_dump($lsb->getDisplayName(false, IntlTimeZone::DISPLAY_SHORT_GENERIC));
var_dump($lsb->getDisplayName(false, IntlTimeZone::DISPLAY_LONG_GENERIC));
var_dump($lsb->getDisplayName(false, IntlTimeZone::DISPLAY_SHORT_GMT));
var_dump($lsb->getDisplayName(false, IntlTimeZone::DISPLAY_LONG_GMT));
var_dump($lsb->getDisplayName(false, IntlTimeZone::DISPLAY_SHORT_COMMONLY_USED));
var_dump($lsb->getDisplayName(false, IntlTimeZone::DISPLAY_GENERIC_LOCATION));

?>
--EXPECT--
string(3) "GMT"
string(30) "Western European Standard Time"
string(13) "Portugal Time"
string(21) "Western European Time"
string(5) "+0000"
string(3) "GMT"
string(3) "GMT"
string(13) "Portugal Time"
