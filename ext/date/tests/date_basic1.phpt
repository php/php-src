--TEST--
Test date() function : basic functionality
--FILE--
<?php
//Set the default time zone
date_default_timezone_set("Europe/London");

echo "*** Testing date() : basic functionality ***\n";

$timestamp = mktime(10, 44, 30, 2, 27, 2009);

var_dump( date("F j, Y, g:i a", $timestamp) );
var_dump( date("m.d.y", $timestamp) );
var_dump( date("j, n, Y", $timestamp) );
var_dump( date("Ymd", $timestamp) );
var_dump( date('h-i-s, j-m-y, it is w Day', $timestamp) );
var_dump( date('\i\t \i\s \t\h\e jS \d\a\y.', $timestamp) );
var_dump( date("D M j G:i:s T Y", $timestamp) );
var_dump( date('H:m:s \m \i\s\ \m\o\n\t\h', $timestamp) );
var_dump( date("H:i:s", $timestamp) );

?>
--EXPECT--
*** Testing date() : basic functionality ***
string(27) "February 27, 2009, 10:44 am"
string(8) "02.27.09"
string(11) "27, 2, 2009"
string(8) "20090227"
string(39) "10-44-30, 27-02-09, 4428 4430 5 Friam09"
string(19) "it is the 27th day."
string(28) "Fri Feb 27 10:44:30 GMT 2009"
string(19) "10:02:30 m is month"
string(8) "10:44:30"
