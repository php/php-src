--TEST--
Test DateTime::setTimezone() function : basic functionality
--FILE--
<?php
echo "*** Testing DateTime::setTimezone() : basic functionality ***\n";

//Set the default time zone
date_default_timezone_set("Europe/London");

$datetime = new DateTime("2009-01-30 17:57:32");
echo "Default timezone: " . date_timezone_get($datetime)->getName() . "\n";

$la_time = new DateTimezone("America/Los_Angeles");
$datetime->setTimezone($la_time);
echo "New timezone: " . date_timezone_get($datetime)->getName() . "\n";

?>
--EXPECT--
*** Testing DateTime::setTimezone() : basic functionality ***
Default timezone: Europe/London
New timezone: America/Los_Angeles
