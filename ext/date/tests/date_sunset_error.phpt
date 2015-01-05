--TEST--
Test date_sunset() function : error conditions 
--FILE--
<?php
/* Prototype  : mixed date_sunset(mixed time [, int format [, float latitude [, float longitude [, float zenith [, float gmt_offset]]]]])
 * Description: Returns time of sunset for a given day and location 
 * Source code: ext/date/php_date.c
 * Alias to functions: 
 */

echo "*** Testing date_sunset() : error conditions ***\n";

//Initialise the variables
$time = time();
$latitude = 38.4;
$longitude = -9;
$zenith = 90;
$gmt_offset = 1;
$extra_arg = 10;

// Zero arguments
echo "\n-- Testing date_sunset() function with Zero arguments --\n";
var_dump( date_sunset() );

//Test date_sunset with one more than the expected number of arguments
echo "\n-- Testing date_sunset() function with more than expected no. of arguments --\n";
var_dump( date_sunset($time, SUNFUNCS_RET_STRING, $latitude, $longitude, $zenith, $gmt_offset, $extra_arg) );
var_dump( date_sunset($time, SUNFUNCS_RET_DOUBLE, $latitude, $longitude, $zenith, $gmt_offset, $extra_arg) );
var_dump( date_sunset($time, SUNFUNCS_RET_TIMESTAMP, $latitude, $longitude, $zenith, $gmt_offset, $extra_arg) );
?>
===DONE===
--EXPECTF--
*** Testing date_sunset() : error conditions ***

-- Testing date_sunset() function with Zero arguments --

Warning: date_sunset() expects at least 1 parameter, 0 given in %s on line %d
bool(false)

-- Testing date_sunset() function with more than expected no. of arguments --

Warning: date_sunset() expects at most 6 parameters, 7 given in %s on line %d
bool(false)

Warning: date_sunset() expects at most 6 parameters, 7 given in %s on line %d
bool(false)

Warning: date_sunset() expects at most 6 parameters, 7 given in %s on line %d
bool(false)
===DONE===
