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
?>
--EXPECT--
IntlTimeZone::createEnumeration(): Argument #1 ($countryOrRawOffset) invalid argument type
