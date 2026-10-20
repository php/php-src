--TEST--
IntlTimeZone::createTimeZoneIDEnumeration() offset out of range
--EXTENSIONS--
intl
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only"); ?>
--FILE--
<?php

try {
	var_dump(IntlTimeZone::createTimeZoneIDEnumeration(IntlTimeZone::TYPE_ANY, '', PHP_INT_MAX));
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
ValueError: IntlTimeZone::createTimeZoneIDEnumeration(): Argument #3 ($rawOffset) must be between -2147483648 and 2147483647
