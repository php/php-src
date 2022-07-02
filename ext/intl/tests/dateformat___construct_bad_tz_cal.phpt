--TEST--
IntlDateFormatter::__construct(): bad timezone or calendar
--EXTENSIONS--
intl
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
} catch (TypeError $e) {
    print_exception($e);
}
?>
--EXPECTF--
Exception: IntlDateFormatter::__construct(): datefmt_create: No such time zone: 'bad timezone' in %s on line %d

Exception: IntlDateFormatter::__construct(): datefmt_create: Invalid value for calendar type; it must be one of IntlDateFormatter::TRADITIONAL (locale's default calendar) or IntlDateFormatter::GREGORIAN. Alternatively, it can be an IntlCalendar object in %s on line %d

Exception: IntlDateFormatter::__construct(): Argument #5 ($calendar) must be of type IntlCalendar|int|null, stdClass given in %s on line %d
