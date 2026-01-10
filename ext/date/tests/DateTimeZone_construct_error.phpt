--TEST--
Test new DateTimeZone(): Too few arguments
--FILE--
<?php
// Set the default time zone
date_default_timezone_set("UTC");

echo "*** Testing DateTimeZone() : error conditions ***\n";

echo "\n-- Testing new DateTimeZone() with more than expected no. of arguments --\n";
$timezone = "GMT";
$extra_arg = 99;
try {
    new DateTimeZone($timezone, $extra_arg);
} catch (ArgumentCountError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
*** Testing DateTimeZone() : error conditions ***

-- Testing new DateTimeZone() with more than expected no. of arguments --
ArgumentCountError: DateTimeZone::__construct() expects exactly 1 argument, 2 given
