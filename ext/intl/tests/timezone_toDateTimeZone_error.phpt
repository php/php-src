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
} catch (\Error $e) {
    echo get_class($e) . ': ' . $e->getMessage();
}

--EXPECTF--
Warning: IntlTimeZone::toDateTimeZone(): intltz_to_date_time_zone: DateTimeZone constructor threw exception in %s on line %d
ValueError: Unknown or bad timezone (Etc/Unknown)
