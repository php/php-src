--TEST--
IntlTimeZone::createEnumeration(): errors
--EXTENSIONS--
intl
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

try {
	IntlTimeZone::createEnumeration(array());
} catch (TypeError $e) {
	echo $e->getMessage() . PHP_EOL;
}

try {
	IntlTimeZone::createEnumeration(str_repeat("8192", 1024));
} catch (ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}

try {
	IntlTimeZone::createEnumeration(31.9998157);
} catch (ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}

try {
	IntlTimeZone::createEnumeration(-2147483649);
} catch (ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}
try {
	IntlTimeZone::createEnumeration(2147483648);
} catch (ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}
?>
--EXPECTF--
IntlTimeZone::createEnumeration(): Argument #1 ($countryOrRawOffset) must be of type string|int, array given

Deprecated: Implicit conversion from float INF to int loses precision in %s on line %d

Deprecated: Implicit conversion from float %f to int loses precision in %s on line %d
IntlTimeZone::createEnumeration(): Argument #1 ($countryOrRawOffset) value must be between %s and %s
IntlTimeZone::createEnumeration(): Argument #1 ($countryOrRawOffset) value must be between %s and %s
