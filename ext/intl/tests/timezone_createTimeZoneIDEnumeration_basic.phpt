--TEST--
IntlTimeZone::createTimeZoneIDEnumeration(): basic test
--EXTENSIONS--
intl
--FILE--
<?php
$enum = IntlTimeZone::createTimeZoneIDEnumeration(
    IntlTimeZone::TYPE_ANY,
    'PT',
    -3600000);
var_dump(iterator_to_array($enum));

$enum = intltz_create_time_zone_id_enumeration(
    IntlTimeZone::TYPE_ANY,
    'PT',
    -3600000);
var_dump(iterator_to_array($enum));
?>
--EXPECT--
array(1) {
  [0]=>
  string(15) "Atlantic/Azores"
}
array(1) {
  [0]=>
  string(15) "Atlantic/Azores"
}
