--TEST--
IntlTimeZone::getDisplayName(): errors
--EXTENSIONS--
intl
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

$tz = IntlTimeZone::createTimeZone('Europe/Lisbon');
var_dump($tz->getDisplayName(false, -1));

?>
--EXPECTF--
Warning: IntlTimeZone::getDisplayName(): wrong display type in %s on line %d
bool(false)
