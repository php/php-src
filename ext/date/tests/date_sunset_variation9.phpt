--TEST--
Test date_sunset() function : usage variation - Passing high positive and negative float values to time argument.
--FILE--
<?php
/* Prototype  : mixed date_sunset(mixed time [, int format [, float latitude [, float longitude [, float zenith [, float gmt_offset]]]]])
 * Description: Returns time of sunset for a given day and location 
 * Source code: ext/date/php_date.c
 * Alias to functions: 
 */

echo "*** Testing date_sunset() : usage variation ***\n";

// GMT is zero for the timezone
date_default_timezone_set("Asia/Calcutta");
//Initialise the variables
$latitude = 38.4;
$longitude = -9;
$zenith = 90;
$gmt_offset = 1;

echo "\n-- Testing date_sunset() function by passing float 12.3456789000e10 value to time --\n";
$time = 12.3456789000e10;
var_dump( date_sunset($time, SUNFUNCS_RET_STRING, $latitude, $longitude, $zenith, $gmt_offset) );
var_dump( date_sunset($time, SUNFUNCS_RET_DOUBLE, $latitude, $longitude, $zenith, $gmt_offset) );
var_dump( date_sunset($time, SUNFUNCS_RET_TIMESTAMP, $latitude, $longitude, $zenith, $gmt_offset) );

echo "\n-- Testing date_sunset() function by passing float -12.3456789000e10 value to time --\n";
$time = -12.3456789000e10;
var_dump( date_sunset($time, SUNFUNCS_RET_STRING, $latitude, $longitude, $zenith, $gmt_offset) );
var_dump( date_sunset($time, SUNFUNCS_RET_DOUBLE, $latitude, $longitude, $zenith, $gmt_offset) );
var_dump( date_sunset($time, SUNFUNCS_RET_TIMESTAMP, $latitude, $longitude, $zenith, $gmt_offset) );

?>
===DONE===
--EXPECTREGEX--
\*\*\* Testing date_sunset\(\) : usage variation \*\*\*

-- Testing date_sunset\(\) function by passing float 12.3456789000e10 value to time --
string\(5\) "(19:49|19:28)"
float\((19.830[0-9]*|19.830[0-9]*|19.480[0-9]*)\)
int\((-1097212211|123456853728)\)

-- Testing date_sunset\(\) function by passing float -12.3456789000e10 value to time --
string\(5\) "(19:03|18:12|18:48)"
float\((19.056[0-9]*|18.213[0-9]*|18.808[0-9]*)\)
int\((1097345002|-2147410031|-123456723090)\)
===DONE===
