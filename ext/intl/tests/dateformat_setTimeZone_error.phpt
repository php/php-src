--TEST--
IntlDateFormatter::setTimeZone() bad args
--EXTENSIONS--
intl
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
ini_set("intl.default_locale", "pt_PT");
ini_set("date.timezone", 'Atlantic/Azores');

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
datefmt_set_timezone: No such time zone: 'Array'
datefmt_set_timezone: No such time zone: 'non existing timezone'
