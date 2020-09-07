--TEST--
Test mktime() function : error conditions
--FILE--
<?php
//Set the default time zone
date_default_timezone_set("Europe/London");

echo "*** Testing mktime() : error conditions ***\n";

echo "\n-- Testing mktime() function with Zero arguments --\n";
try {
    var_dump( mktime() );
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

echo "\n-- Testing mktime() function with more than expected no. of arguments --\n";
$hour = 10;
$minute = 30;
$sec = 45;
$month = 7;
$day = 2;
$year = 1963;
$extra_arg = 10;
try {
    var_dump( mktime($hour, $minute, $sec, $month, $day, $year, $extra_arg) );
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
*** Testing mktime() : error conditions ***

-- Testing mktime() function with Zero arguments --
mktime() expects at least 1 argument, 0 given

-- Testing mktime() function with more than expected no. of arguments --
mktime() expects at most 6 arguments, 7 given
