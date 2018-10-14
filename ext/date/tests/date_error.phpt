--TEST--
Test date() function : error conditions
--FILE--
<?php
/* Prototype  : string date  ( string $format  [, int $timestamp  ] )
 * Description: Format a local time/date.
 * Source code: ext/date/php_date.c
 */

echo "*** Testing date() : error conditions ***\n";

//Set the default time zone
date_default_timezone_set("America/Chicago");

$format = "m.d.y";
$timestamp = mktime(10, 44, 30, 2, 27, 2009);

echo "\n-- Testing date function with no arguments --\n";
var_dump (date());

echo "\n-- Testing date function with more than expected no. of arguments --\n";
$extra_arg = true;
var_dump (checkdate($format, $timestamp, $extra_arg));

?>
===DONE===
--EXPECTF--
*** Testing date() : error conditions ***

-- Testing date function with no arguments --

Warning: date() expects at least 1 parameter, 0 given in %s on line %d
bool(false)

-- Testing date function with more than expected no. of arguments --

Warning: checkdate() expects parameter 1 to be integer, string given in %s on line %d
bool(false)
===DONE===
