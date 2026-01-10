--TEST--
IntlTimeZone::createTimeZoneIDEnumeration() invalid zone type
--EXTENSIONS--
intl
--FILE--
<?php

try {
	var_dump(IntlTimeZone::createTimeZoneIDEnumeration(-1));
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
ValueError: IntlTimeZone::createTimeZoneIDEnumeration(): Argument #1 ($type) must be one of IntlTimeZone::TYPE_ANY, IntlTimeZone::TYPE_CANONICAL, or IntlTimeZone::TYPE_CANONICAL_LOCATION
