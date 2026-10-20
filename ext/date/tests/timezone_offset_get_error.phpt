--TEST--
Test timezone_offset_get() function : error conditions
--FILE--
<?php
//Set the default time zone
date_default_timezone_set("GMT");
$tz = timezone_open("Europe/London");
$date = date_create("GMT");

echo "*** Testing timezone_offset_get() : error conditions ***\n";

echo "\n-- Testing timezone_offset_get() function with an invalid values for \$object argument --\n";
$invalid_obj = new stdClass();
try {
    var_dump( timezone_offset_get($invalid_obj, $date) );
} catch (Error $ex) {
    echo $ex::class, ': ', $ex->getMessage(), "\n";
}
$invalid_obj = 10;
try {
    var_dump( timezone_offset_get($invalid_obj, $date) );
} catch (Error $ex) {
    echo $ex::class, ': ', $ex->getMessage(), "\n";
}
$invalid_obj = null;
try {
    var_dump( timezone_offset_get($invalid_obj, $date) );
} catch (Error $ex) {
    echo $ex::class, ': ', $ex->getMessage(), "\n";
}

echo "\n-- Testing timezone_offset_get() function with an invalid values for \$datetime argument --\n";
$invalid_obj = new stdClass();
try {
    var_dump( timezone_offset_get($tz, $invalid_obj) );
} catch (Error $ex) {
    echo $ex::class, ': ', $ex->getMessage(), "\n";
}
$invalid_obj = 10;
try {
    var_dump( timezone_offset_get($tz, $invalid_obj) );
} catch (Error $ex) {
    echo $ex::class, ': ', $ex->getMessage(), "\n";
}
$invalid_obj = null;
try {
    var_dump( timezone_offset_get($tz, $invalid_obj) );
} catch (Error $ex) {
    echo $ex::class, ': ', $ex->getMessage(), "\n";
}
?>
--EXPECT--
*** Testing timezone_offset_get() : error conditions ***

-- Testing timezone_offset_get() function with an invalid values for $object argument --
TypeError: timezone_offset_get(): Argument #1 ($object) must be of type DateTimeZone, stdClass given
TypeError: timezone_offset_get(): Argument #1 ($object) must be of type DateTimeZone, int given
TypeError: timezone_offset_get(): Argument #1 ($object) must be of type DateTimeZone, null given

-- Testing timezone_offset_get() function with an invalid values for $datetime argument --
TypeError: timezone_offset_get(): Argument #2 ($datetime) must be of type DateTimeInterface, stdClass given
TypeError: timezone_offset_get(): Argument #2 ($datetime) must be of type DateTimeInterface, int given
TypeError: timezone_offset_get(): Argument #2 ($datetime) must be of type DateTimeInterface, null given
