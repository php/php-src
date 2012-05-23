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

var_dump(IntlTimeZone::fromDateTimeZone());
var_dump(IntlTimeZone::fromDateTimeZone(1,2));
var_dump(IntlTimeZone::fromDateTimeZone('sdfds'));
var_dump(IntlTimeZone::fromDateTimeZone(new stdclass));
$dt = new DateTime('2012-08-01 00:00:00 WEST');
var_dump(IntlTimeZone::fromDateTimeZone($dt->getTimeZone()));

var_dump(intltz_from_date_time_zone());

--EXPECTF--

Warning: IntlTimeZone::fromDateTimeZone() expects exactly 1 parameter, 0 given in %s on line %d

Warning: IntlTimeZone::fromDateTimeZone(): intltz_from_date_time_zone: bad arguments in %s on line %d
NULL

Warning: IntlTimeZone::fromDateTimeZone() expects exactly 1 parameter, 2 given in %s on line %d

Warning: IntlTimeZone::fromDateTimeZone(): intltz_from_date_time_zone: bad arguments in %s on line %d
NULL

Warning: IntlTimeZone::fromDateTimeZone() expects parameter 1 to be DateTimeZone, string given in %s on line %d

Warning: IntlTimeZone::fromDateTimeZone(): intltz_from_date_time_zone: bad arguments in %s on line %d
NULL

Warning: IntlTimeZone::fromDateTimeZone() expects parameter 1 to be DateTimeZone, object given in %s on line %d

Warning: IntlTimeZone::fromDateTimeZone(): intltz_from_date_time_zone: bad arguments in %s on line %d
NULL

Warning: IntlTimeZone::fromDateTimeZone(): intltz_from_date_time_zone: time zone id 'WEST' extracted from ext/date DateTimeZone not recognized in %s on line %d
NULL

Warning: intltz_from_date_time_zone() expects exactly 1 parameter, 0 given in %s on line %d

Warning: intltz_from_date_time_zone(): intltz_from_date_time_zone: bad arguments in %s on line %d
NULL
