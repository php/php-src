--TEST--
Test date_sunrise() function : usage variation -  Passing high positive and negative float values to time argument.
--FILE--
<?php
/* Prototype  : mixed date_sunrise(mixed time [, int format [, float latitude [, float longitude [, float zenith [, float gmt_offset]]]]])
 * Description: Returns time of sunrise for a given day and location 
 * Source code: ext/date/php_date.c
 * Alias to functions: 
 */

echo "*** Testing date_sunrise() : usage variation ***\n";

// GMT is zero for the timezone
date_default_timezone_set("Asia/Calcutta");
//Initialise the variables
$latitude = 38.4;
$longitude = -9;
$zenith = 90;
$gmt_offset = 1;

echo "\n-- Testing date_sunrise() function by passing float 12.3456789000e10 value to time --\n";
$time = 12.3456789000e10;
var_dump( date_sunrise($time, SUNFUNCS_RET_STRING, $latitude, $longitude, $zenith, $gmt_offset) );
var_dump( date_sunrise($time, SUNFUNCS_RET_DOUBLE, $latitude, $longitude, $zenith, $gmt_offset) );
var_dump( date_sunrise($time, SUNFUNCS_RET_TIMESTAMP, $latitude, $longitude, $zenith, $gmt_offset) );

echo "\n-- Testing date_sunrise() function by passing float -12.3456789000e10 value to time --\n";
$time = -12.3456789000e10;
var_dump( date_sunrise($time, SUNFUNCS_RET_STRING, $latitude, $longitude, $zenith, $gmt_offset) );
var_dump( date_sunrise($time, SUNFUNCS_RET_DOUBLE, $latitude, $longitude, $zenith, $gmt_offset) );
var_dump( date_sunrise($time, SUNFUNCS_RET_TIMESTAMP, $latitude, $longitude, $zenith, $gmt_offset) );

?>
===DONE===
--EXPECTREGEX--
\*\*\* Testing date_sunrise\(\) : usage variation \*\*\*

-- Testing date_sunrise\(\) function by passing float 12.3456789000e10 value to time --
string\(5\) "(07:34|07:49)"
float\((7.566[0-9]*|7.821[0-9]*)\)
int\((-1097256359|123456811756)\)

-- Testing date_sunrise\(\) function by passing float -12.3456789000e10 value to time --
string\(5\) "(07:42|08:48|08:04)"
float\((7.713[0-9]*|8.810[0-9]*|8.074[0-9]*)\)
int\((1097304168|-2147443882|-123456761731)\)
===DONE===
