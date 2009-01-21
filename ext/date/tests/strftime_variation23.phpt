--TEST--
Test strftime() function : usage variation - Checking large positive and negative float values to timestamp.
--FILE--
<?php
/* Prototype  : string strftime(string format [, int timestamp])
 * Description: Format a local time/date according to locale settings 
 * Source code: ext/date/php_date.c
 * Alias to functions: 
 */

echo "*** Testing strftime() : usage variation ***\n";

// Initialise function arguments not being substituted (if any)
locale_set_default("en_US");
date_default_timezone_set("UTC");
$format = '%b %d %Y %H:%M:%S';

echo "\n-- Testing strftime() function with float 12.3456789000e10 to timestamp --\n";
$timestamp = 12.3456789000e10;
var_dump( strftime($format, $timestamp) );

echo "\n-- Testing strftime() function with float -12.3456789000e10 to timestamp --\n";
$timestamp = -12.3456789000e10;
var_dump( strftime($format, $timestamp) );

?>
===DONE===
--EXPECTREGEX--
\*\*\* Testing strftime\(\) : usage variation \*\*\*

-- Testing strftime\(\) function with float 12.3456789000e10 to timestamp --
unicode\(\d*\)\s"(Jan|Mar)\s(19|11)\s(2038|5882)\s(03|00):(14|30):(07|00)"

-- Testing strftime\(\) function with float -12.3456789000e10 to timestamp --
unicode\(\d*\)\s"(Dec|Oct)\s(13|22)\s(1901|-1943)\s(20|23):(45|30):(52|00)"
===DONE===
