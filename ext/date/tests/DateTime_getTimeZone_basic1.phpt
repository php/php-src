--TEST--
Test DateTime::getTimezone() function : basic functionality
--FILE--
<?php
echo "*** Testing DateTime::getTimezone() : basic functionality ***\n";

date_default_timezone_set("Europe/London");
$object = new DateTime("2009-01-30 17:57:32");
var_dump( $object->getTimeZone()->getName() );


date_default_timezone_set("America/New_York");
$object = new DateTime("2009-01-30 17:57:32");
var_dump( $object->getTimeZone()->getName() );

$la_time = new DateTimeZone("America/Los_Angeles");
$object->setTimeZone($la_time);
var_dump( $object->getTimeZone()->getName() );

?>
--EXPECT--
*** Testing DateTime::getTimezone() : basic functionality ***
string(13) "Europe/London"
string(16) "America/New_York"
string(19) "America/Los_Angeles"
