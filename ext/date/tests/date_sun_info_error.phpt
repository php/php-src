--TEST--
Test date_sun_info() function : error variations
--FILE--
<?php
/* Prototype  : array date_sun_info ( int $time , float $latitude , float $longitude   )
 * Description:  Returns an array with information about sunset/sunrise and twilight begin/end.
 * Source code: ext/standard/data/php_date.c
 */

echo "*** Testing date_sun_info() : usage variations ***\n";

$time = "2006-12-12";
$latitude=31.7667;
$longitude=35.2333;

echo "\n-- Testing date_sun_info() function with less than expected no. of arguments --\n";
var_dump( date_sun_info() );
var_dump( date_sun_info($time) );
var_dump( date_sun_info($time, $latitude) );


echo "\n-- Testing date_sun_info() function with more than expected no. of arguments --\n";
$extra_arg = 99;
var_dump( date_create($time, $latitude, $longitude, $extra_arg) );

?>
===Done===
--EXPECTF--
*** Testing date_sun_info() : usage variations ***

-- Testing date_sun_info() function with less than expected no. of arguments --

Warning: date_sun_info() expects exactly 3 parameters, 0 given in %s on line %d
bool(false)

Warning: date_sun_info() expects exactly 3 parameters, 1 given in %s on line %d
bool(false)

Warning: date_sun_info() expects exactly 3 parameters, 2 given in %s on line %d
bool(false)

-- Testing date_sun_info() function with more than expected no. of arguments --

Warning: date_create() expects at most 2 parameters, 4 given in %s on line %d
bool(false)
===Done===