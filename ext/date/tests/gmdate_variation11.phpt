--TEST--
Test gmdate() function : usage variation - Passing Full Date/Time format options to format argument.
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

echo "\n-- Testing gmdate() function with ISO 8601 date format --\n";
var_dump( gmdate('c') );
var_dump( gmdate('c', $timestamp) );

echo "\n-- Testing gmdate() function with RFC 2822 date format --\n";
var_dump( gmdate('r') );
var_dump( gmdate('r', $timestamp) );

echo "\n-- Testing gmdate() function with seconds since Unix Epoch format --\n";
var_dump( gmdate('U') );
var_dump( gmdate('U', $timestamp) );

?>
===DONE===
--EXPECTF--
*** Testing gmdate() : usage variation ***

-- Testing gmdate() function with ISO 8601 date format --
string(%d) "%s"
string(25) "2008-08-08T08:08:08+00:00"

-- Testing gmdate() function with RFC 2822 date format --
string(%d) "%s"
string(31) "Fri, 08 Aug 2008 08:08:08 +0000"

-- Testing gmdate() function with seconds since Unix Epoch format --
string(%d) "%d"
string(10) "1218182888"
===DONE===
