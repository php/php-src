--TEST--
IntlDateFormatter::__construct(): bad timezone or calendar
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
ini_set("intl.default_locale", "pt_PT");
ini_set("date.timezone", 'Atlantic/Azores');

function print_exception($e) {
	echo "\nException: " . $e->getMessage() . " in " . $e->getFile() . " on line " . $e->getLine() . "\n";
}

try {
	var_dump(new IntlDateFormatter(NULL, 0, 0, 'bad timezone'));
} catch (IntlException $e) {
	print_exception($e);
}
try {
	var_dump(new IntlDateFormatter(NULL, 0, 0, NULL, 3));
} catch (IntlException $e) {
	print_exception($e);
}
try {
	var_dump(new IntlDateFormatter(NULL, 0, 0, NULL, new stdclass));
} catch (IntlException $e) {
	print_exception($e);
}
?>
==DONE==
--EXPECTF--
Exception: IntlDateFormatter::__construct(): datefmt_create: no such time zone: 'bad timezone' in %s on line %d

Exception: IntlDateFormatter::__construct(): datefmt_create: invalid value for calendar type; it must be one of IntlDateFormatter::TRADITIONAL (locale's default calendar) or IntlDateFormatter::GREGORIAN. Alternatively, it can be an IntlCalendar object in %s on line %d

Exception: IntlDateFormatter::__construct(): datefmt_create: Invalid calendar argument; should be an integer or an IntlCalendar instance in %s on line %d
==DONE==
