--TEST--
Test timezone_name_from_abbr() function : basic functionality
--FILE--
<?php
/* Prototype  : string timezone_name_from_abbr  ( string $abbr  [, int $gmtOffset= -1  [, int $isdst= -1  ]] )
 * Description: Returns the timezone name from abbrevation
 * Source code: ext/date/php_date.c
 * Alias to functions:
 */

echo "*** Testing timezone_name_from_abbr() : basic functionality ***\n";

//Set the default time zone
date_default_timezone_set("Europe/London");

echo "-- Tests with special cases first - no lookup needed --\n";
var_dump( timezone_name_from_abbr("GMT") );
var_dump( timezone_name_from_abbr("UTC") );

echo "-- Lookup with just name --\n";
var_dump( timezone_name_from_abbr("CET") );
var_dump( timezone_name_from_abbr("EDT") );

echo "-- Lookup with name and offset--\n";
var_dump( timezone_name_from_abbr("ADT", -10800) );
var_dump( timezone_name_from_abbr("ADT", 14400) );

echo "-- Tests without valid name - uses gmtOffset and isdst to find match --\n";
var_dump( timezone_name_from_abbr("", 3600, 1) );
var_dump( timezone_name_from_abbr("FOO", -7200, 1) );
var_dump( timezone_name_from_abbr("", -14400, 1) );
var_dump( timezone_name_from_abbr("", -14400, 0) );

echo "-- Tests with invalid offsets --\n";
var_dump( timezone_name_from_abbr("", 5400) ); // offset = 1.5 hrs
var_dump( timezone_name_from_abbr("", 62400) ); // offset = 24 hrs
?>
===DONE===
--EXPECTF--
*** Testing timezone_name_from_abbr() : basic functionality ***
-- Tests with special cases first - no lookup needed --
string(3) "UTC"
string(3) "UTC"
-- Lookup with just name --
string(13) "Europe/Berlin"
string(16) "America/New_York"
-- Lookup with name and offset--
string(15) "America/Halifax"
string(15) "America/Halifax"
-- Tests without valid name - uses gmtOffset and isdst to find match --
string(13) "Europe/London"
string(17) "America/Sao_Paulo"
string(16) "America/New_York"
string(15) "America/Halifax"
-- Tests with invalid offsets --
bool(false)
bool(false)
===DONE===
