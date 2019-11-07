--TEST--
Test timezone_offset_get() function : error conditions
--FILE--
<?php
/* Prototype  : int timezone_offset_get  ( DateTimeZone $object  , DateTime $datetime  )
 * Description: Returns the timezone offset from GMT
 * Source code: ext/date/php_date.c
 * Alias to functions: DateTimeZone::getOffset
 */

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
string(74) "timezone_offset_get() expects parameter 1 to be DateTimeZone, object given"

string(71) "timezone_offset_get() expects parameter 1 to be DateTimeZone, int given"

string(72) "timezone_offset_get() expects parameter 1 to be DateTimeZone, null given"


-- Testing timezone_offset_get() function with an invalid values for $datetime argument --
string(79) "timezone_offset_get() expects parameter 2 to be DateTimeInterface, object given"

string(76) "timezone_offset_get() expects parameter 2 to be DateTimeInterface, int given"

string(77) "timezone_offset_get() expects parameter 2 to be DateTimeInterface, null given"

