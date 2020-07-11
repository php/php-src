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
    var_dump($ex->getMessage());
    echo "\n";
}
$invalid_obj = 10;
try {
    var_dump( timezone_offset_get($invalid_obj, $date) );
} catch (Error $ex) {
    var_dump($ex->getMessage());
    echo "\n";
}
$invalid_obj = null;
try {
    var_dump( timezone_offset_get($invalid_obj, $date) );
} catch (Error $ex) {
    var_dump($ex->getMessage());
    echo "\n";
}

echo "\n-- Testing timezone_offset_get() function with an invalid values for \$datetime argument --\n";
$invalid_obj = new stdClass();
try {
    var_dump( timezone_offset_get($tz, $invalid_obj) );
} catch (Error $ex) {
    var_dump($ex->getMessage());
    echo "\n";
}
$invalid_obj = 10;
try {
    var_dump( timezone_offset_get($tz, $invalid_obj) );
} catch (Error $ex) {
    var_dump($ex->getMessage());
    echo "\n";
}
$invalid_obj = null;
try {
    var_dump( timezone_offset_get($tz, $invalid_obj) );
} catch (Error $ex) {
    var_dump($ex->getMessage());
    echo "\n";
}
?>
--EXPECT--
*** Testing timezone_offset_get() : error conditions ***

-- Testing timezone_offset_get() function with an invalid values for $object argument --
string(89) "timezone_offset_get(): Argument #1 ($object) must be of type DateTimeZone, stdClass given"

string(84) "timezone_offset_get(): Argument #1 ($object) must be of type DateTimeZone, int given"

string(85) "timezone_offset_get(): Argument #1 ($object) must be of type DateTimeZone, null given"


-- Testing timezone_offset_get() function with an invalid values for $datetime argument --
string(96) "timezone_offset_get(): Argument #2 ($datetime) must be of type DateTimeInterface, stdClass given"

string(91) "timezone_offset_get(): Argument #2 ($datetime) must be of type DateTimeInterface, int given"

string(92) "timezone_offset_get(): Argument #2 ($datetime) must be of type DateTimeInterface, null given"
