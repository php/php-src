--TEST--
Test date_format() function : error conditions
--FILE--
<?php
/* Prototype  : string date_format  ( DateTime $object  , string $format  )
 * Description: Returns date formatted according to given format
 * Source code: ext/date/php_date.c
 * Alias to functions: DateTime::format
 */

//Set the default time zone
date_default_timezone_set("Europe/London");

echo "*** Testing date_format() : error conditions ***\n";

echo "\n-- Testing date_create() function with zero arguments --\n";
var_dump( date_format() );

$date = date_create("2005-07-14 22:30:41");

echo "\n-- Testing date_create() function with less than expected no. of arguments --\n";
var_dump( date_format($date) );

echo "\n-- Testing date_create() function with more than expected no. of arguments --\n";
$format = "F j, Y, g:i a";
$extra_arg = 10;
var_dump( date_format($date, $format, $extra_arg) );

echo "\n-- Testing date_create() function with an invalid values for \$object argument --\n";
$invalid_obj = new stdClass();
var_dump( date_format($invalid_obj, $format) );
$invalid_obj = 10;
var_dump( date_format($invalid_obj, $format) );
$invalid_obj = null;
var_dump( date_format($invalid_obj, $format) );

?>
===DONE===
--EXPECTF--
*** Testing date_format() : error conditions ***

-- Testing date_create() function with zero arguments --

Warning: date_format() expects exactly 2 parameters, 0 given in %s on line %d
bool(false)

-- Testing date_create() function with less than expected no. of arguments --

Warning: date_format() expects exactly 2 parameters, 1 given in %s on line %d
bool(false)

-- Testing date_create() function with more than expected no. of arguments --

Warning: date_format() expects exactly 2 parameters, 3 given in %s on line %d
bool(false)

-- Testing date_create() function with an invalid values for $object argument --

Warning: date_format() expects parameter 1 to be DateTimeInterface, object given in %sp on line %d
bool(false)

Warning: date_format() expects parameter 1 to be DateTimeInterface, integer given in %s on line %d
bool(false)

Warning: date_format() expects parameter 1 to be DateTimeInterface, null given in %s on line %d
bool(false)
===DONE===
