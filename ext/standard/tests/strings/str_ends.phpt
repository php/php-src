--TEST--
Test str_ends() family functions
--FILE--
<?php

echo "*** Testing str_ends() ***\n";

echo "-- With null arguments --\n";
var_dump( str_ends(null, "abc") );
var_dump( str_ends("abc", null) );

echo "-- With too long pattern --\n";
var_dump( str_ends("cdefghi", "abc") );

echo "-- Case sensitiveness --\n";
var_dump( str_ends("DEF", "abcdef") );
var_dump( str_ends("cdeF", "aBcdef") );

echo "-- Correct patterns --\n";
var_dump( str_ends("f", "abcdef") );
var_dump( str_ends("cdef", "aBcdef") );

echo "*** Testing str_iends() ***\n";

echo "-- With null arguments --\n";
var_dump( str_iends(null, "abc") );
var_dump( str_iends("abc", null) );

echo "-- With too long pattern --\n";
var_dump( str_iends("cDefg", "abc") );

echo "-- Correct patterns --\n";
var_dump( str_iends("F", "abcdef") );
var_dump( str_iends("cDEF", "aBcDef") );

echo "Done";

?>
--EXPECT--
*** Testing str_ends() ***
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
*** Testing str_iends() ***
-- With null arguments --
bool(false)
bool(false)
-- With too long pattern --
bool(false)
-- Correct patterns --
bool(true)
bool(true)
Done
