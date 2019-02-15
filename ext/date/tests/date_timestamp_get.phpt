--TEST--
DateTime: Test correct setup and correct DateTime parameter to date_timestamp_get()
--CREDITS--
Havard Eide <nucleuz@gmail.com>
#PHPTestFest2009 Norway 2009-06-09 \o/
--INI--
date.timezone=UTC
--FILE--
<?php
$tz = date_timestamp_get(new DateTime());
var_dump(is_int($tz));
echo "\n\n";
$tz = date_timestamp_get(time());
?>
--EXPECTF--
bool(true)



Warning: date_timestamp_get() expects parameter 1 to be DateTimeInterface, int given in %s on line %d
