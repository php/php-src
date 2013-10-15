--TEST--
IntlTimeZone::createTimeZoneIDEnumeration(): variant without offset
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
if (version_compare(INTL_ICU_VERSION, '4.8') < 0)
	die('skip for ICU 4.8+');
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
==DONE==
--EXPECT--
bool(true)
bool(true)
bool(true)
==DONE==