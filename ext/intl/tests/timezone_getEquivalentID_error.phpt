--TEST--
IntlTimeZone::getEquivalentID(): errors
--EXTENSIONS--
intl
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

var_dump(IntlTimeZone::getEquivalentID("foo\x80", 0));

try {
	IntlTimeZOne::getEquivalentID("NL", -2147483649);
} catch (ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}

try {
	IntlTimeZOne::getEquivalentID("NL", 2147483648);
} catch (ValueError $e) {
	echo $e->getMessage();
}
?>
--EXPECTF--
Warning: IntlTimeZone::getEquivalentID(): could not convert time zone id to UTF-16 in %s on line %d
bool(false)
IntlTimeZone::getEquivalentID(): Argument #2 ($offset) index must be between %s and %s
IntlTimeZone::getEquivalentID(): Argument #2 ($offset) index must be between %s and %s
