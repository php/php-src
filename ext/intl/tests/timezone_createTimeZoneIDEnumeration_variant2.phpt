--TEST--
IntlTimeZone::createTimeZoneIDEnumeration(): variant without region
--SKIPIF--
<?php
if (!extension_loaded('intl'))
    die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
$enum = IntlTimeZone::createTimeZoneIDEnumeration(
    IntlTimeZone::TYPE_ANY);
$countAny = count(iterator_to_array($enum));
$enum = IntlTimeZone::createTimeZoneIDEnumeration(
    IntlTimeZone::TYPE_CANONICAL);
$countCanonical = count(iterator_to_array($enum));
$enum = IntlTimeZone::createTimeZoneIDEnumeration(
    IntlTimeZone::TYPE_CANONICAL_LOCATION);
$countCanonicalLocation = count(iterator_to_array($enum));

var_dump($countAny > $countCanonical);
var_dump($countCanonical > $countCanonicalLocation);

$enum = IntlTimeZone::createTimeZoneIDEnumeration(
    IntlTimeZone::TYPE_ANY, null, null);
$countAny2 = count(iterator_to_array($enum));
var_dump($countAny == $countAny2);

$enum = IntlTimeZone::createTimeZoneIDEnumeration(
    IntlTimeZone::TYPE_ANY, null, -3600000);
$values = iterator_to_array($enum);

print_r(
array_values(
array_intersect($values,
array('Etc/GMT+1', 'Atlantic/Azores'))
));


?>
--EXPECT--
bool(true)
bool(true)
bool(true)
Array
(
    [0] => Atlantic/Azores
    [1] => Etc/GMT+1
)
