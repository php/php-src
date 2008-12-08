--TEST--
Test idate() function : usage variation - Checking for Strict Standards.
--FILE--
<?php
/* Prototype  : int idate(string format [, int timestamp])
 * Description: Format a local time/date as integer 
 * Source code: ext/date/php_date.c
 * Alias to functions: 
 */

echo "*** Testing idate() : usage variation ***\n";

// Initialise function arguments not being substituted (if any)
$format = 'Y';
$timestamp = 1218163088;

var_dump( idate($format) );
var_dump( idate($format, $timestamp) );
?>
===DONE===
--EXPECTF--
*** Testing idate() : usage variation ***

Strict Standards: idate(): It is not safe to rely on the system's timezone settings.%s
int(2008)

Strict Standards: idate(): It is not safe to rely on the system's timezone settings.%s
int(2008)
===DONE===
