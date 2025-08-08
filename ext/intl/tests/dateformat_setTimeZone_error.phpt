--TEST--
IntlDateFormatter::setTimeZone() bad args
--EXTENSIONS--
intl
--INI--
intl.default_locale=pt_PT
date.timezone=Atlantic/Azores
--FILE--
<?php

$df = new IntlDateFormatter(NULL, 0, 0);

try {
	$df->setTimeZone(array());
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
	$df->setTimeZone('non existing timezone');
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
TypeError: IntlDateFormatter::setTimeZone(): Argument #1 ($timezone) must be of type object|string|null, array given
IntlException: IntlDateFormatter::setTimeZone(): No such time zone: "non existing timezone"
