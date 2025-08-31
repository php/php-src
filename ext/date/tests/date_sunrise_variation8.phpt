--TEST--
Test date_sunrise() function : usage variation -  Checking with North and South poles when Sun is up and down all day
--INI--
error_reporting=E_ALL&~E_DEPRECATED
--FILE--
<?php
echo "*** Testing date_sunrise() : usage variation ***\n";

// GMT is zero for the timezone
date_default_timezone_set("Africa/Casablanca");
$time_date = array (

        //Date at which Sun is up all day at North Pole
        "12 Aug 2008" => mktime(8, 8, 8, 8, 12, 2008),
        "13 Aug 2008" => mktime(8, 8, 8, 8, 13, 2008),

        //Date at which Sun is up all day at South Pole
        "12 Nov 2008" => mktime(8, 8, 8, 11, 12, 2008),
        "13 Nov 2008" => mktime(8, 8, 8, 11, 13, 2008),
);

//Iterate over different date and time
foreach( $time_date as $date => $time ){
    echo "\n--$date--\n";
    var_dump( date_sunrise($time, SUNFUNCS_RET_STRING, 90, 0 ) );
    var_dump( date_sunrise($time, SUNFUNCS_RET_DOUBLE, 90, 0 ) );
    var_dump( date_sunrise($time, SUNFUNCS_RET_TIMESTAMP, 90, 0 ) );
    var_dump( date_sunrise($time, SUNFUNCS_RET_STRING, -90, 0 ) );
    var_dump( date_sunrise($time, SUNFUNCS_RET_DOUBLE, -90, 0 ) );
    var_dump( date_sunrise($time, SUNFUNCS_RET_TIMESTAMP, -90, 0 ) );
}

?>
--EXPECT--
*** Testing date_sunrise() : usage variation ***

--12 Aug 2008--
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)

--13 Aug 2008--
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)

--12 Nov 2008--
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)

--13 Nov 2008--
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
