--TEST--
Test strrchr() function : basic functionality
--FILE--
<?php
echo "*** Testing strrchr() function: basic functionality ***\n";
var_dump( strrchr("Hello, World", "H") ); //needle as single char
var_dump( strrchr("Hello, World", "H", true) ); //needle as single char
var_dump( strrchr("Hello, World", "Hello") ); //needle as a first word of haystack
var_dump( strrchr("Hello, World", "Hello", true) ); //needle as a first word of haystack
var_dump( strrchr('Hello, World', 'H') );
var_dump( strrchr('Hello, World', 'H', true) );
var_dump( strrchr('Hello, World', 'Hello') );
var_dump( strrchr('Hello, World', 'Hello', true) );

//considering case
var_dump( strrchr("Hello, World", "h") );
var_dump( strrchr("Hello, World", "h", true) );
var_dump( strrchr("Hello, World", "hello") );
var_dump( strrchr("Hello, World", "hello", true) );

//needle as second word of haystack
var_dump( strrchr("Hello, World", "World") );
var_dump( strrchr("Hello, World", "World", true) );
var_dump( strrchr('Hello, World', 'World') );
var_dump( strrchr('Hello, World', 'World', true) );

//needle as special char
var_dump( strrchr("Hello, World", ",") );
var_dump( strrchr("Hello, World", ",", true) );
var_dump( strrchr('Hello, World', ',') );
var_dump( strrchr('Hello, World', ',', true) );

var_dump( strrchr("Hello, World", "Hello, World") ); //needle as haystack
var_dump( strrchr("Hello, World", "Hello, World", true) ); //needle as haystack

//needle string containing one existing and one non-existing char
var_dump( strrchr("Hello, World", "Hi") );
var_dump( strrchr("Hello, World", "Hi", true) );

//multiple existence of needle in haystack
var_dump( strrchr("Hello, World", "o") );
var_dump( strrchr("Hello, World", "o", true) );
var_dump( strrchr("Hello, World", "ooo") );
var_dump( strrchr("Hello, World", "ooo", true) );

var_dump( strrchr("Hello, World", "Zzzz") ); //non-existent needle in haystack
var_dump( strrchr("Hello, World", "Zzzz", true) ); //non-existent needle in haystack
echo "*** Done ***";
?>
--EXPECT--
*** Testing strrchr() function: basic functionality ***
string(12) "Hello, World"
string(0) ""
string(12) "Hello, World"
string(0) ""
string(12) "Hello, World"
string(0) ""
string(12) "Hello, World"
string(0) ""
bool(false)
bool(false)
bool(false)
bool(false)
string(5) "World"
string(7) "Hello, "
string(5) "World"
string(7) "Hello, "
string(7) ", World"
string(5) "Hello"
string(7) ", World"
string(5) "Hello"
string(12) "Hello, World"
string(0) ""
string(12) "Hello, World"
string(0) ""
string(4) "orld"
string(8) "Hello, W"
string(4) "orld"
string(8) "Hello, W"
bool(false)
bool(false)
*** Done ***
