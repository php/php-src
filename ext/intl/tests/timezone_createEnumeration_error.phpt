--TEST--
IntlTimeZone::createEnumeration(): errors
--EXTENSIONS--
intl
--FILE--
<?php

try {
	var_dump(IntlTimeZone::createEnumeration([]));
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
	var_dump(IntlTimeZone::createEnumeration(new stdClass()));
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
TypeError: IntlTimeZone::createEnumeration(): Argument #1 ($countryOrRawOffset) must be of type string|int|null, array given
TypeError: IntlTimeZone::createEnumeration(): Argument #1 ($countryOrRawOffset) must be of type string|int|null, stdClass given
