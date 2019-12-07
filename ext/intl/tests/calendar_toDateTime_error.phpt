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
} catch (\Error $e) {
    echo get_class($e) . ': ' . $e->getMessage();
}

--EXPECTF--
Warning: IntlCalendar::toDateTime(): intlcal_to_date_time: DateTimeZone constructor threw exception in %s on line %d
ValueError: Unknown or bad timezone (Etc/Unknown)

