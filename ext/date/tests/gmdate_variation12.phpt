--TEST--
Test gmdate() function : usage variation - Valid and invalid range of timestamp.
--FILE--
<?php
/* Prototype  : string gmdate(string format [, long timestamp])
 * Description: Format a GMT date/time
 * Source code: ext/date/php_date.c
 * Alias to functions:
 */

echo "*** Testing gmdate() : usage variation ***\n";

// Initialise all required variables
date_default_timezone_set('UTC');

$timestamp = mktime(20, 45, 54, 12, 13, 1901);
echo "\n-- Testing gmdate() function with minimum range of timestamp --\n";
var_dump( gmdate(DATE_ISO8601, $timestamp) );

$timestamp = mktime(20, 45, 50, 12, 13, 1901);
echo "\n-- Testing gmdate() function with less than the range of timestamp --\n";
var_dump( gmdate(DATE_ISO8601, $timestamp) );

echo "\n-- Testing gmdate() function with maximum range of timestamp --\n";
$timestamp = mktime(03, 14, 07, 1, 19, 2038);
var_dump( gmdate(DATE_ISO8601, $timestamp) );

echo "\n-- Testing gmdate() function with greater than the range of timestamp --\n";
$timestamp = mktime(03, 14, 10, 1, 19, 2038);
var_dump( gmdate(DATE_ISO8601, $timestamp) );

?>
===DONE===
--EXPECTREGEX--
\*\*\* Testing gmdate\(\) : usage variation \*\*\*

-- Testing gmdate\(\) function with minimum range of timestamp --
string\(24\) "1901-12-13T20:45:54\+0000"

-- Testing gmdate\(\) function with less than the range of timestamp --
string\(24\) "(1970-01-01T00:00:00\+0000|1901-12-13T20:45:50\+0000)"

-- Testing gmdate\(\) function with maximum range of timestamp --
string\(24\) "2038-01-19T03:14:07\+0000"

-- Testing gmdate\(\) function with greater than the range of timestamp --
string\(24\) "(1970-01-01T00:00:00\+0000|2038-01-19T03:14:10\+0000)"
===DONE===
