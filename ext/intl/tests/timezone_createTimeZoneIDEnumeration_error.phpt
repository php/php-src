--TEST--
IntlTimeZone::createTimeZoneIDEnumeration(): errors
--EXTENSIONS--
intl
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

var_dump(IntlTimeZone::createTimeZoneIDEnumeration(-1));

try {
	IntlTimeZone::createTimeZoneIDEnumeration(IntlTimeZone::TYPE_ANY, null, -2147483649);
} catch (ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}
try {
	IntlTimeZone::createTimeZoneIDEnumeration(IntlTimeZone::TYPE_ANY, null, 2147483648);
} catch (ValueError $e) {
	echo $e->getMessage();
}
?>
--EXPECTF--
Warning: IntlTimeZone::createTimeZoneIDEnumeration(): bad zone type in %s on line %d
bool(false)
IntlTimeZone::createTimeZoneIDEnumeration(): Argument #3 ($rawOffset) offset must be between %s and %s
IntlTimeZone::createTimeZoneIDEnumeration(): Argument #3 ($rawOffset) offset must be between %s and %s
