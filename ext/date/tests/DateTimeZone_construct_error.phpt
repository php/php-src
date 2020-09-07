--TEST--
Test new DateTimeZone() : error conditions
--FILE--
<?php
//Set the default time zone
date_default_timezone_set("GMT");

echo "*** Testing DateTimeZone() : error conditions ***\n";

echo "\n-- Testing new DateTimeZone() with more than expected no. of arguments --\n";
$timezone = "GMT";
$extra_arg = 99;
try {
    new DateTimeZone($timezone, $extra_arg);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
*** Testing DateTimeZone() : error conditions ***

-- Testing new DateTimeZone() with more than expected no. of arguments --
DateTimeZone::__construct() expects exactly 1 argument, 2 given
