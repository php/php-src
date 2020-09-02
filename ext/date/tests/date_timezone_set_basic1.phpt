--TEST--
Test date_timezone_set() function : basic functionality
--FILE--
<?php
echo "*** Testing date_timezone_set() : basic functionality ***\n";

//Set the default time zone
date_default_timezone_set("Europe/London");

$datetime = date_create("2009-01-30 17:57:32");
$tz = date_timezone_get($datetime);
echo "Default timezone: " . timezone_name_get($tz) . "\n";

$datetime = date_create("2009-01-30 22:57:32");
$la_time = timezone_open("America/Los_Angeles");

date_timezone_set($datetime, $la_time);
$tz = date_timezone_get($datetime);
echo "New timezone: " . timezone_name_get($tz) . "\n";

?>
--EXPECT--
*** Testing date_timezone_set() : basic functionality ***
Default timezone: Europe/London
New timezone: America/Los_Angeles
