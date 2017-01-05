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

set_error_handler('err');

function err($errno, $errstr) {
	if ($errno === E_RECOVERABLE_ERROR) {
		var_dump($errstr);
	}
}

echo "*** Testing timezone_offset_get() : error conditions ***\n";

echo "\n-- Testing timezone_offset_get() function with zero arguments --\n";
try {
	var_dump( timezone_offset_get() );
} catch (Error $ex) {
	var_dump($ex->getMessage());
	echo "\n";
}

echo "\n-- Testing timezone_offset_get() function with less than expected no. of arguments --\n";
try {
	var_dump( timezone_offset_get($tz) );
} catch (Error $ex) {
	var_dump($ex->getMessage());
	echo "\n";
}

echo "\n-- Testing timezone_offset_get() function with more than expected no. of arguments --\n";
$extra_arg = 99;
try {
	var_dump( timezone_offset_get($tz, $date, $extra_arg) );
} catch (Error $ex) {
	var_dump($ex->getMessage());
	echo "\n";
}

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
===DONE===
--EXPECTF--
*** Testing timezone_offset_get() : error conditions ***

-- Testing timezone_offset_get() function with zero arguments --
bool(false)

-- Testing timezone_offset_get() function with less than expected no. of arguments --
bool(false)

-- Testing timezone_offset_get() function with more than expected no. of arguments --
bool(false)

-- Testing timezone_offset_get() function with an invalid values for $object argument --
string(%d) "Argument 1 passed to timezone_offset_get() must be an instance of DateTimeZone, instance of stdClass given"

string(%d) "Argument 1 passed to timezone_offset_get() must be an instance of DateTimeZone, integer given"

string(%d) "Argument 1 passed to timezone_offset_get() must be an instance of DateTimeZone, null given"


-- Testing timezone_offset_get() function with an invalid values for $datetime argument --
string(%d) "Argument 2 passed to timezone_offset_get() must implement interface DateTimeInterface, instance of stdClass given"

string(%d) "Argument 2 passed to timezone_offset_get() must implement interface DateTimeInterface, integer given"

string(%d) "Argument 2 passed to timezone_offset_get() must implement interface DateTimeInterface, null given"

===DONE===
