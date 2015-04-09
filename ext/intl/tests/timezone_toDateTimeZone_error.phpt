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

var_dump($tz->toDateTimeZone(''));
try {
	var_dump($tz->toDateTimeZone());
} catch (Exception $e) {
	var_dump($e->getMessage());
}

var_dump(intltz_to_date_time_zone());
var_dump(intltz_to_date_time_zone(1));

--EXPECTF--

Warning: IntlTimeZone::toDateTimeZone() expects exactly 0 parameters, 1 given in %s on line %d

Warning: IntlTimeZone::toDateTimeZone(): intltz_to_date_time_zone: bad arguments in %s on line %d
bool(false)

Warning: IntlTimeZone::toDateTimeZone(): intltz_to_date_time_zone: DateTimeZone constructor threw exception in %s on line %d
string(66) "DateTimeZone::__construct(): Unknown or bad timezone (Etc/Unknown)"

Warning: intltz_to_date_time_zone() expects exactly 1 parameter, 0 given in %s on line %d

Warning: intltz_to_date_time_zone(): intltz_to_date_time_zone: bad arguments in %s on line %d
bool(false)

Fatal error: Argument 1 passed to intltz_to_date_time_zone() must be an instance of IntlTimeZone, integer given in %s on line %d
