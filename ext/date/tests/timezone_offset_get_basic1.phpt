--TEST--
Test timezone_offset_get() function : basic functionality
--FILE--
<?php
echo "*** Testing timezone_offset_get() : basic functionality ***\n";

//Set the default time zone
date_default_timezone_set("GMT");

$tz = timezone_open("Europe/London");
$date = date_create("GMT");

var_dump(timezone_offset_get($tz, $date));

$tz = timezone_open("America/New_York");
var_dump(timezone_offset_get($tz, $date));

$tz = timezone_open("America/Los_Angeles");
var_dump(timezone_offset_get($tz, $date));

?>
--EXPECTF--
*** Testing timezone_offset_get() : basic functionality ***
%rint\(0\)|int\(3600\)%r
%rint\(-18000\)|int\(-14400\)%r
%rint\(-28800\)|int\(-25200\)%r
