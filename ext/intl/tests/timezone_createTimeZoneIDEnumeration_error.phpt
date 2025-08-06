--TEST--
IntlTimeZone::createTimeZoneIDEnumeration(): errors
--EXTENSIONS--
intl
--FILE--
<?php

try {
	var_dump(IntlTimeZone::createTimeZoneIDEnumeration(-1));
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
	var_dump(IntlTimeZone::createTimeZoneIDEnumeration(IntlTimeZone::TYPE_ANY, '', PHP_INT_MAX));
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
ValueError: IntlTimeZone::createTimeZoneIDEnumeration(): Argument #1 ($type) must be one of IntlTimeZone::TYPE_ANY, IntlTimeZone::TYPE_CANONICAL, or IntlTimeZone::TYPE_CANONICAL_LOCATION
ValueError: IntlTimeZone::createTimeZoneIDEnumeration(): Argument #1 ($type) must be between -2147483648 and 2147483647
