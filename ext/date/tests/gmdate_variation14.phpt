--TEST--
Test gmdate() function : usage variation - Passing high positive and negetive float values to timestamp.
--FILE--
<?php
/* Prototype  : string gmdate(string format [, long timestamp])
 * Description: Format a GMT date/time 
 * Source code: ext/date/php_date.c
 */

echo "*** Testing gmdate() : usage variation ***\n";

// Initialise all required variables
date_default_timezone_set('UTC');
$format = DATE_ISO8601;

echo "\n-- Testing gmdate() function with float 12.3456789000e10 to timestamp --\n";
$timestamp = 12.3456789000e10;
var_dump( gmdate($format, $timestamp) );

echo "\n-- Testing gmdate() function with float -12.3456789000e10 to timestamp --\n";
$timestamp = -12.3456789000e10;
var_dump( gmdate($format, $timestamp) );

?>
===DONE===
--EXPECTREGEX--
\*\*\* Testing gmdate\(\) : usage variation \*\*\*

-- Testing gmdate\(\) function with float 12.3456789000e10 to timestamp --
string\((24|25)\) "(1935-03-26T04:50:16\+0000|5882-03-11T00:30:00\+0000)"

-- Testing gmdate\(\) function with float -12.3456789000e10 to timestamp --
string\((24|25)\) "(2004-10-08T19:09:44\+0000|1901-12-13T20:45:52\+0000|-1943-10-22T23:30:00\+0000)"
===DONE===