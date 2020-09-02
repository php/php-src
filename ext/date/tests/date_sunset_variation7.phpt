--TEST--
Test date_sunset() function : usage variation -  Checking sunrise for consecutive days in specific timezone
--FILE--
<?php
echo "*** Testing date_sunset() : usage variation ***\n";

//Timezones with required data for date_sunrise
$inputs = array (
        //Timezone with Latitude, Longitude and GMT offset
        "Pacific/Samoa" => array ("Latitude" => -14.24, "Longitude" => -170.72, "GMT" => -11),
        "US/Alaska" => array ("Latitude" => 61, "Longitude" => -150 , "GMT" => -9),
        "America/Chicago" => array ("Latitude" => 41.85, "Longitude" => -87.65 , "GMT" => -5),
        "America/Montevideo" => array ("Latitude" => -34.88, "Longitude" => -56.18 , "GMT" => -3),
        "Africa/Casablanca" => array ("Latitude" => 33.65, "Longitude" => "-7.58", "GMT" => 0),
        "Europe/Moscow" => array ("Latitude" => 55.75, "Longitude" => 37.58, "GMT" => 4),
        "Asia/Hong_Kong" => array ("Latitude" => 22.28, "Longitude" => 114.15 , "GMT" => 8),
        "Australia/Brisbane" => array ("Latitude" => -27.46, "Longitude" => 153.2 , "GMT" => 10),
        "Pacific/Wallis" => array ("Latitude" => -13.3, "Longitude" => -176.16, "GMT" => 12),
);

foreach($inputs as $timezone => $value) {
     echo "\n--$timezone--\n";
     date_default_timezone_set($timezone);
     $time = mktime(8, 8, 8, 8, 11, 2008);
     var_dump( date_sunset($time, SUNFUNCS_RET_STRING, $value["Latitude"], $value["Longitude"], 90, $value["GMT"] ));
     $time = mktime(8, 8, 8, 8, 12, 2008);
     var_dump( date_sunset($time, SUNFUNCS_RET_STRING, $value["Latitude"], $value["Longitude"], 90, $value["GMT"]) );
}
?>
--EXPECT--
*** Testing date_sunset() : usage variation ***

--Pacific/Samoa--
string(5) "18:13"
string(5) "18:13"

--US/Alaska--
string(5) "21:02"
string(5) "20:59"

--America/Chicago--
string(5) "19:52"
string(5) "19:51"

--America/Montevideo--
string(5) "18:08"
string(5) "18:08"

--Africa/Casablanca--
string(5) "19:18"
string(5) "19:17"

--Europe/Moscow--
string(5) "21:10"
string(5) "21:08"

--Asia/Hong_Kong--
string(5) "18:55"
string(5) "18:54"

--Australia/Brisbane--
string(5) "17:21"
string(5) "17:21"

--Pacific/Wallis--
string(5) "17:36"
string(5) "17:36"
