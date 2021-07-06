--TEST--
IntlTimeZone::createTimeZoneIDEnumeration(): variant without offset
--EXTENSIONS--
intl
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
$enum = IntlTimeZone::createTimeZoneIDEnumeration(
    IntlTimeZone::TYPE_ANY,
    'PT');
$values = iterator_to_array($enum);
var_dump(in_array('Europe/Lisbon', $values));
var_dump(in_array('Atlantic/Azores', $values));

$enum = IntlTimeZone::createTimeZoneIDEnumeration(
    IntlTimeZone::TYPE_ANY,
    'PT',
    null);
$values2 = iterator_to_array($enum);
var_dump($values2 == $values);

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
