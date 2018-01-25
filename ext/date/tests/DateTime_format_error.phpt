--TEST--
Test DateTime::format() function : error conditions
--FILE--
<?php
/* Prototype  : public string DateTime::format  ( string $format  )
 * Description: Returns date formatted according to given format
 * Source code: ext/date/php_date.c
 * Alias to functions: date_format
 */

//Set the default time zone
date_default_timezone_set("Europe/London");

// Craete a date object
$date = new DateTime("2005-07-14 22:30:41");

echo "*** Testing DateTime::format() : error conditions ***\n";

echo "\n-- Testing date_date_formatcreate() function with zero arguments --\n";
var_dump( $date->format() );

echo "\n-- Testing date_date_formatcreate() function with more than expected no. of arguments --\n";
$format = "F j, Y, g:i a";
$extra_arg = 10;
var_dump( $date->format($format, $extra_arg) );

?>
===DONE===
--EXPECTF--
*** Testing DateTime::format() : error conditions ***

-- Testing date_date_formatcreate() function with zero arguments --

Warning: DateTime::format() expects exactly 1 parameter, 0 given in %s on line %d
bool(false)

-- Testing date_date_formatcreate() function with more than expected no. of arguments --

Warning: DateTime::format() expects exactly 1 parameter, 2 given in %s on line %d
bool(false)
===DONE===
