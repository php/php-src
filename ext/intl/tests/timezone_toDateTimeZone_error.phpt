--TEST--
IntlTimeZone::toDateTimeZone(): errors
--SKIPIF--
<?php
if (!extension_loaded('intl'))
    die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

$tz = IntlTimeZone::createTimeZone('Etc/Unknown');

try {
    var_dump($tz->toDateTimeZone());
} catch (Exception $e) {
    var_dump($e->getMessage());
}

var_dump(intltz_to_date_time_zone(1));
?>
--EXPECTF--
Warning: IntlTimeZone::toDateTimeZone(): intltz_to_date_time_zone: DateTimeZone constructor threw exception in %s on line %d
string(66) "DateTimeZone::__construct(): Unknown or bad timezone (Etc/Unknown)"

Fatal error: Uncaught TypeError: intltz_to_date_time_zone(): Argument #1 ($timezone) must be of type IntlTimeZone, int given in %s:%d
Stack trace:
#0 %s(%d): intltz_to_date_time_zone(1)
#1 {main}
  thrown in %s on line %d
