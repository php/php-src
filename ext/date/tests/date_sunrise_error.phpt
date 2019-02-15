--TEST--
Test date_sunrise() function : error conditions
--FILE--
<?php
/* Prototype  : mixed date_sunrise(mixed time [, int format [, float latitude [, float longitude [, float zenith [, float gmt_offset]]]]])
 * Description: Returns time of sunrise for a given day and location
 * Source code: ext/date/php_date.c
 */

echo "*** Testing date_sunrise() : error conditions ***\n";

//Initialise the variables
$time = time();
$latitude = 38.4;
$longitude = -9;
$zenith = 90;
$gmt_offset = 1;
$extra_arg = 10;

// Zero arguments
echo "\n-- Testing date_sunrise() function with Zero arguments --\n";
var_dump( date_sunrise() );

//Test date_sunrise with one more than the expected number of arguments
echo "\n-- Testing date_sunrise() function with more than expected no. of arguments --\n";
var_dump( date_sunrise($time, SUNFUNCS_RET_STRING, $latitude, $longitude, $zenith, $gmt_offset, $extra_arg) );
?>
===DONE===
--EXPECTF--
*** Testing date_sunrise() : error conditions ***

-- Testing date_sunrise() function with Zero arguments --

Warning: date_sunrise() expects at least 1 parameter, 0 given in %s on line %d
bool(false)

-- Testing date_sunrise() function with more than expected no. of arguments --

Warning: date_sunrise() expects at most 6 parameters, 7 given in %s on line %d
bool(false)
===DONE===
