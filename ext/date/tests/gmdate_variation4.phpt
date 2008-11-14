--TEST--
Test gmdate() function : usage variation - Passing textual representation of day formats.
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

echo "\n-- Testing gmdate() function with partial textual representation of day --\n";
var_dump( gmdate('D') );
var_dump( gmdate('D', $timestamp) );

echo "\n-- Testing gmdate() function with full textual representation of day --\n";
var_dump( gmdate('l') );
var_dump( gmdate('l', $timestamp) );

echo "\n-- Testing gmdate() function with English ordinal suffix --\n";
var_dump( gmdate('S') );
var_dump( gmdate('S', $timestamp) );

?>
===DONE===
--EXPECTF--
*** Testing gmdate() : usage variation ***

-- Testing gmdate() function with partial textual representation of day --
string(%d) "%s"
string(3) "Fri"

-- Testing gmdate() function with full textual representation of day --
string(%d) "%s"
string(6) "Friday"

-- Testing gmdate() function with English ordinal suffix --
string(%d) "%s"
string(2) "th"
===DONE===
