--TEST--
Basic test for date_sunrise and date_sunset based on example in PHP manual
--FILE--
<?php

date_default_timezone_set('UTC');

/* calculate the sunrise time for Lisbon, Portugal
Latitude: 38.4 North
Longitude: 9 West
Zenith ~= 90
offset: +1 GMT
*/

echo "Basic test for date_sunrise() and date_sunset()\n";

// suppress date() function strict msgs
error_reporting(E_ALL & ~E_STRICT);

echo date("D M d Y") . ', sunrise time : ' . date_sunrise(time(), SUNFUNCS_RET_STRING, 38.4, -9, 90, 1) . "\n";
echo date("D M d Y") . ', sunset time : ' . date_sunset(time(), SUNFUNCS_RET_STRING, 38.4, -9, 90, 1) . "\n";

?>
===Done===
--EXPECTF--
Basic test for date_sunrise() and date_sunset()
%s %s %d %d, sunrise time : %d:%d
%s %s %d %d, sunset time : %d:%d
===Done===
