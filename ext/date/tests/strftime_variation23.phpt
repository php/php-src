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
setlocale(LC_ALL, "en_US");
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
string\(\d*\)\s"Mar\s(26|11)\s(1935|5882)\s(04|00):(50|30):(16|00)"

-- Testing strftime\(\) function with float -12.3456789000e10 to timestamp --
string\(\d*\)\s"(Oct|Dec)\s(08|13|22)\s(2004|1901|-1943)\s(19|20|23):(09|45|30):(44|52|00)"
===DONE===
