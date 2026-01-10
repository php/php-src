--TEST--
IntlTimeZone::createTimeZoneIDEnumeration(): variant without region
--EXTENSIONS--
intl
--FILE--
<?php
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

var_dump(
	array_values(array_intersect($values, ['Etc/GMT+1', 'Atlantic/Azores'])
));


?>
--EXPECT--
bool(true)
bool(true)
bool(true)
array(2) {
  [0]=>
  string(15) "Atlantic/Azores"
  [1]=>
  string(9) "Etc/GMT+1"
}
