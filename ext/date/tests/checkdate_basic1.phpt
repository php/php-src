--TEST--
Test date_create() function : basic functionality
--FILE--
<?php
/* Prototype  : bool checkdate  ( int $month  , int $day  , int $year  )
 * Description: Checks the validity of the date formed by the arguments.
 *              A date is considered valid if each parameter is properly defined.
 * Source code: ext/date/php_date.c
 */

//Set the default time zone
date_default_timezone_set("Europe/London");

echo "*** Testing checkdate() : basic functionality ***\n";

echo "-- The following are all valid dates --\n";
var_dump( checkdate(1, 1, 2009) );
var_dump( checkdate(12, 31, 2009) );
var_dump( checkdate(7, 2, 1963) );
var_dump( checkdate(5, 31, 2009) );
var_dump( checkdate(2, 28, 2009) ); // non-leap year
var_dump( checkdate(2, 29, 2008) ); // leap year
var_dump( checkdate(7, 2, 1) );     // min year
var_dump( checkdate(7, 2, 32767) ); // max year

echo "-- The following are all invalid dates --\n";
var_dump( checkdate(13, 1, 2009) );
var_dump( checkdate(2, 31, 2009) );
var_dump( checkdate(1, 32, 2009) );
var_dump( checkdate(2, 29, 2009) ); // non-leap year
var_dump( checkdate(7, 2, 32768) ); // >max year
var_dump( checkdate(7, 2, 0) ); // <min year

?>
===DONE===
--EXPECT--
*** Testing checkdate() : basic functionality ***
-- The following are all valid dates --
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
-- The following are all invalid dates --
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
===DONE===
