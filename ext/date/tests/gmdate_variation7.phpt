--TEST--
Test gmdate() function : usage variation - Passing Year format options to format argument.
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
$timestamp = mktime(8, 8, 8, 8, 8, 2008);
$timestamp_non_leap_year = mktime(8, 8, 8, 8, 8, 2007);

echo "\n-- Testing gmdate() function with checking non leap year using Leap Year format --\n";
var_dump( gmdate('L', $timestamp_non_leap_year) );

echo "\n-- Testing gmdate() function with checking leap year using Leap Year format --\n";
var_dump( gmdate('L') );
var_dump( gmdate('L', $timestamp) );

echo "\n-- Testing gmdate() function with ISO-8601 year number format --\n";
var_dump( gmdate('o') );
var_dump( gmdate('o', $timestamp) );

echo "\n-- Testing gmdate() function with full numeric representation of year format --\n";
var_dump( gmdate('Y') );
var_dump( gmdate('Y', $timestamp) );

echo "\n-- Testing gmdate() function with 2 digit representation year format --\n";
var_dump( gmdate('y') );
var_dump( gmdate('y', $timestamp) );

?>
===DONE===
--EXPECTF--
*** Testing gmdate() : usage variation ***

-- Testing gmdate() function with checking non leap year using Leap Year format --
string(1) "0"

-- Testing gmdate() function with checking leap year using Leap Year format --
string(1) "%d"
string(1) "1"

-- Testing gmdate() function with ISO-8601 year number format --
string(4) "%d"
string(4) "2008"

-- Testing gmdate() function with full numeric representation of year format --
string(4) "%d"
string(4) "2008"

-- Testing gmdate() function with 2 digit representation year format --
string(2) "%d"
string(2) "08"
===DONE===
