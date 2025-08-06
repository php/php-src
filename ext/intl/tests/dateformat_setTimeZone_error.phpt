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
} catch (IntlException $e) {
	echo $e->getMessage() . PHP_EOL;
}

try {
	$df->setTimeZone('non existing timezone');
} catch (IntlException $e) {
	echo $e->getMessage();
}
?>
--EXPECTF--
Warning: Array to string conversion in %s on line %d
IntlDateFormatter::setTimeZone(): No such time zone: "Array"
IntlDateFormatter::setTimeZone(): No such time zone: "non existing timezone"
