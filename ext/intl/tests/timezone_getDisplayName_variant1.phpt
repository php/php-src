--TEST--
IntlTimeZone::getDisplayName(): daylight parameter effect
--EXTENSIONS--
intl
--INI--
intl.default_locale=en_US
--FILE--
<?php

$lsb = IntlTimeZone::createTimeZone('Europe/Lisbon');

var_dump($lsb->getDisplayName());
var_dump($lsb->getDisplayName(false));
var_dump($lsb->getDisplayName(true));

?>
--EXPECTF--
string(%d) "Western European%sTime"
string(%d) "Western European%sTime"
string(28) "Western European Summer Time"
