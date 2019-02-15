--TEST--
Test date_timezone_get() function : basic functionality
--FILE--
<?php
/* Prototype  : DateTimeZone date_timezone_get  ( DateTime $object  )
 * Description: Return time zone relative to given DateTime
 * Source code: ext/date/php_date.c
 * Alias to functions: DateTime::setTimezone
 */

echo "*** Testing date_timezone_get() : basic functionality ***\n";

date_default_timezone_set("Europe/London");
$object = date_create("2009-01-30 17:57:32");
$tz = date_timezone_get($object);
var_dump( timezone_name_get($tz) );

date_default_timezone_set("America/New_York");
$object = date_create("2009-01-30 17:57:32");
$tz = date_timezone_get($object);
var_dump( timezone_name_get($tz) );

$la_time = timezone_open("America/Los_Angeles");
date_timezone_set($object, $la_time);
$tz = date_timezone_get($object);
var_dump( timezone_name_get($tz) );

?>
===DONE===
--EXPECT--
*** Testing date_timezone_get() : basic functionality ***
string(13) "Europe/London"
string(16) "America/New_York"
string(19) "America/Los_Angeles"
===DONE===
