--TEST--
IntlTimeZone::fromDateTimeZone(): argument errors
--INI--
date.timezone=Atlantic/Azores
--EXTENSIONS--
intl
--FILE--
<?php

$dt = new DateTime('2012-08-01 00:00:00 WEST');
var_dump(IntlTimeZone::fromDateTimeZone($dt->getTimeZone()));
var_dump(intl_get_error_message());

?>
--EXPECT--
NULL
string(131) "IntlTimeZone::fromDateTimeZone(): time zone id 'WEST' extracted from ext/date DateTimeZone not recognized: U_ILLEGAL_ARGUMENT_ERROR"
