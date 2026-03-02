--TEST--
Test mktime() function : basic functionality
--FILE--
<?php
//Set the default time zone
date_default_timezone_set("Europe/London");

echo "*** Testing DateTime::modify() : basic functionality ***\n";

$hour = 10;
$minute = 30;
$sec = 45;
$month = 7;
$day = 2;
$year = 1963;

var_dump( mktime($hour) );
var_dump( mktime($hour, $minute) );
var_dump( mktime($hour, $minute, $sec) );
var_dump( mktime($hour, $minute, $sec, $month) );
var_dump( mktime($hour, $minute, $sec, $month, $day) );
var_dump( mktime($hour, $minute, $sec, $month, $day, $year) );

?>
--EXPECTF--
*** Testing DateTime::modify() : basic functionality ***
int(%i)
int(%i)
int(%i)
int(%i)
int(%i)
int(%i)
