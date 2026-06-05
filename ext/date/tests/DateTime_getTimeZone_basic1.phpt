--TEST--
Test DateTime::getTimezone() function : basic functionality
--FILE--
<?php
echo "*** Testing DateTime::getTimezone() : basic functionality ***\n";

date_default_timezone_set("Europe/London");
$object = new DateTime("2009-01-30 17:57:32");
var_dump( $object->getTimezone()->getName() );


date_default_timezone_set("America/New_York");
$object = new DateTime("2009-01-30 17:57:32");
var_dump( $object->getTimezone()->getName() );

$la_time = new DateTimeZone("America/Los_Angeles");
$object->setTimezone($la_time);
var_dump( $object->getTimezone()->getName() );

?>
--EXPECT--
*** Testing DateTime::getTimezone() : basic functionality ***
string(13) "Europe/London"
string(16) "America/New_York"
string(19) "America/Los_Angeles"
