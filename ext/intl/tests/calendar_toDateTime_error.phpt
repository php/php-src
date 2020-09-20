--TEST--
IntlCalendar::toDateTime(): bad arguments
--SKIPIF--
<?php
if (!extension_loaded('intl'))
    die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
ini_set('date.timezone', 'Europe/Lisbon');

$cal = new IntlGregorianCalendar("Etc/Unknown");
try {
var_dump($cal->toDateTime());
} catch (Exception $e) {
var_dump("exception: {$e->getMessage()}");
}

try {
    var_dump(intlcal_to_date_time($cal));
} catch (\Exception $e) {
    var_dump($e->getMessage());
}

$cal = IntlCalendar::createInstance("Etc/Unknown");
try {
    var_dump($cal->toDateTime());
} catch (\Exception $e) {
    var_dump($e->getMessage());
}

try {
    var_dump(intlcal_to_date_time($cal));
} catch (\Exception $e) {
    var_dump($e->getMessage());
}

try {
    var_dump(intlcal_to_date_time(3));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
?>
--EXPECTF--
Warning: IntlCalendar::toDateTime(): intlcal_to_date_time: DateTimeZone constructor threw exception in %s on line %d
string(77) "exception: DateTimeZone::__construct(): Unknown or bad timezone (Etc/Unknown)"

Warning: intlcal_to_date_time(): intlcal_to_date_time: DateTimeZone constructor threw exception in %s on line %d
string(66) "DateTimeZone::__construct(): Unknown or bad timezone (Etc/Unknown)"

Warning: IntlCalendar::toDateTime(): intlcal_to_date_time: DateTimeZone constructor threw exception in %s on line %d
string(66) "DateTimeZone::__construct(): Unknown or bad timezone (Etc/Unknown)"

Warning: intlcal_to_date_time(): intlcal_to_date_time: DateTimeZone constructor threw exception in %s on line %d
string(66) "DateTimeZone::__construct(): Unknown or bad timezone (Etc/Unknown)"
intlcal_to_date_time(): Argument #1 ($calendar) must be of type IntlCalendar, int given
