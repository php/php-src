--TEST--
IntlTimeZone::getDisplayName(): type parameter (ICU < 49)
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
if (version_compare(INTL_ICU_VERSION, '49') >= 0)
	die('skip for ICU < 49');
if (version_compare(INTL_ICU_VERSION, '4.8') < 0)
	die('skip for ICU 4.8+');
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
==DONE==
--EXPECT--
string(3) "WET"
string(21) "Western European Time"
string(22) "Portugal Time (Lisbon)"
string(22) "Portugal Time (Lisbon)"
string(5) "+0000"
string(3) "GMT"
string(3) "GMT"
string(22) "Portugal Time (Lisbon)"
==DONE==