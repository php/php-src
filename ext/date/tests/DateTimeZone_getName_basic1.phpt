--TEST--
Test DateTimeZone::getName() function : basic functionality
--FILE--
<?php
echo "*** Testing DateTimeZone::getName() : basic functionality ***\n";

//Set the default time zone
date_default_timezone_set("GMT");

$tz1 = new DateTimeZone("Europe/London");
var_dump( $tz1->getName() );

$tz2 = new DateTimeZone("America/New_York");
var_dump( $tz2->getName() );

$tz3 = new DateTimeZone("America/Los_Angeles");
var_dump( $tz3->getName() );

?>
--EXPECT--
*** Testing DateTimeZone::getName() : basic functionality ***
string(13) "Europe/London"
string(16) "America/New_York"
string(19) "America/Los_Angeles"
