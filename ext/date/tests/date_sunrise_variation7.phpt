--TEST--
Test date_sunrise() function : usage variation -  Checking sunrise for consecutive days in specific timezone
--FILE--
<?php
/* Prototype  : mixed date_sunrise(mixed time [, int format [, float latitude [, float longitude [, float zenith [, float gmt_offset]]]]])
 * Description: Returns time of sunrise for a given day and location 
 * Source code: ext/date/php_date.c
 * Alias to functions: 
 */

echo "*** Testing date_sunrise() : usage variation ***\n";

//Timezones with required data for date_sunrise
$inputs = array (
		//Timezone with Latitude, Longitude and GMT offset
		"Pacific/Samoa" => array ("Latitude" => -14.24, "Longitude" => -170.72, "GMT" => -11),
		"US/Alaska" => array ("Latitude" => 61, "Longitude" => -150 , "GMT" => -9),
		"America/Chicago" => array ("Latitude" => 41.85, "Longitude" => -87.65 , "GMT" => -5),
		"America/Montevideo" => array ("Latitude" => -34.88, "Longitude" => -56.18 , "GMT" => -3),
		"Africa/Casablanca" => array ("Latitude" => 33.65, "Longitude" => -7.58, "GMT" => 0),
		"Europe/Moscow" => array ("Latitude" => 55.75, "Longitude" => 37.58, "GMT" => 4),
		"Asia/Hong_Kong" => array ("Latitude" => 22.28, "Longitude" => 114.15 , "GMT" => 8),
		"Australia/Brisbane" => array ("Latitude" => -27.46, "Longitude" => 153.2 , "GMT" => 10),
		"Pacific/Wallis" => array ("Latitude" => -13.3, "Longitude" => -176.16, "GMT" => 12),
);

foreach($inputs as $timezone => $value) {
	 date_default_timezone_set($timezone);
	 $time = mktime(8, 8, 8, 8, 11, 2008);
	 var_dump( date_sunrise($time, SUNFUNCS_RET_STRING, $value["Latitude"], $value["Longitude"], 90, $value["GMT"] ));
	 $time = mktime(8, 8, 8, 8, 12, 2008); 
	 var_dump( date_sunrise($time, SUNFUNCS_RET_STRING, $value["Latitude"], $value["Longitude"], 90, $value["GMT"]) );
}

?>
===DONE===
--EXPECTF--
*** Testing date_sunrise() : usage variation ***
string(5) "06:41"
string(5) "06:41"
string(5) "05:09"
string(5) "05:11"
string(5) "05:59"
string(5) "06:00"
string(5) "07:30"
string(5) "07:29"
string(5) "05:53"
string(5) "05:53"
string(5) "05:59"
string(5) "06:01"
string(5) "06:01"
string(5) "06:02"
string(5) "06:23"
string(5) "06:22"
string(5) "06:03"
string(5) "06:02"
===DONE===
