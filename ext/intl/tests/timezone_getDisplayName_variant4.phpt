--TEST--
IntlTimeZone::getDisplayName(): type parameter (ICU >= 51.2)
--EXTENSIONS--
intl
--INI--
intl.default_locale=en_US
--FILE--
<?php

$lsb = IntlTimeZone::createTimeZone('Europe/Lisbon');

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
