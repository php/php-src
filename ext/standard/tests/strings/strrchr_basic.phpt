--TEST--
Test strrchr() function : basic functionality
--FILE--
<?php
echo "*** Testing strrchr() function: basic functionality ***\n";
var_dump( strrchr("Hello, World", "H") ); //needle as single char
var_dump( strrchr("Hello, World", "Hello") ); //needle as a first word of haystack
var_dump( strrchr('Hello, World', 'H') );
var_dump( strrchr('Hello, World', 'Hello') );

//considering case
var_dump( strrchr("Hello, World", "h") );
var_dump( strrchr("Hello, World", "hello") );

//needle as second word of haystack
var_dump( strrchr("Hello, World", "World") );
var_dump( strrchr('Hello, World', 'World') );

//needle as special char
var_dump( strrchr("Hello, World", ",") );
var_dump( strrchr('Hello, World', ',') );

var_dump( strrchr("Hello, World", "Hello, World") ); //needle as haystack

//needle string containing one existing and one non-existing char
var_dump( strrchr("Hello, World", "Hi") );

//multiple existence of needle in haystack
var_dump( strrchr("Hello, World", "o") );
var_dump( strrchr("Hello, World", "ooo") );

var_dump( strrchr("Hello, World", "Zzzz") ); //non-existent needle in haystack
echo "*** Done ***";
?>
--EXPECT--
*** Testing strrchr() function: basic functionality ***
string(12) "Hello, World"
string(12) "Hello, World"
string(12) "Hello, World"
string(12) "Hello, World"
bool(false)
bool(false)
string(5) "World"
string(5) "World"
string(7) ", World"
string(7) ", World"
string(12) "Hello, World"
string(12) "Hello, World"
string(4) "orld"
string(4) "orld"
bool(false)
*** Done ***
