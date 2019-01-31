--TEST--
Test str_begins() family functions
--FILE--
<?php

echo "*** Testing str_begins() ***\n";

echo "-- With null arguments --\n";
var_dump( str_begins(null, "abc") );
var_dump( str_begins("abc", null) );

echo "-- With too long pattern --\n";
var_dump( str_begins("abcdef", "abc") );

echo "-- Case sensitiveness --\n";
var_dump( str_begins("Abc", "abcdef") );
var_dump( str_begins("abc", "aBcdef") );

echo "-- Correct patterns --\n";
var_dump( str_begins("abc", "abcdef") );
var_dump( str_begins("aB", "aBcdef") );

echo "*** Testing str_ibegins() ***\n";

echo "-- With null arguments --\n";
var_dump( str_ibegins(null, "abc") );
var_dump( str_ibegins("abc", null) );

echo "-- With too long pattern --\n";
var_dump( str_ibegins("aBcdef", "abc") );

echo "-- Correct patterns --\n";
var_dump( str_ibegins("aBC", "abcdef") );
var_dump( str_ibegins("ab", "aBcDef") );

echo "Done";

?>
--EXPECT--
*** Testing str_begins() ***
-- With null arguments --
bool(false)
bool(false)
-- With too long pattern --
bool(false)
-- Case sensitiveness --
bool(false)
bool(false)
-- Correct patterns --
bool(true)
bool(true)
*** Testing str_ibegins() ***
-- With null arguments --
bool(false)
bool(false)
-- With too long pattern --
bool(false)
-- Correct patterns --
bool(true)
bool(true)
Done
