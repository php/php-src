--TEST--
IntlTimeZone::fromDateTimeZone(): argument errors
--INI--
date.timezone=Atlantic/Azores
--SKIPIF--
<?php
if (!extension_loaded('intl'))
    die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

$dt = new DateTime('2012-08-01 00:00:00 WEST');
var_dump(IntlTimeZone::fromDateTimeZone($dt->getTimeZone()));
?>
--EXPECTF--
Warning: IntlTimeZone::fromDateTimeZone(): intltz_from_date_time_zone: time zone id 'WEST' extracted from ext/date DateTimeZone not recognized in %s on line %d
NULL
