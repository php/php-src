--TEST--
Test strrchr() function : basic functionality 
--FILE--
<?php
/* Prototype  : string strrchr(string $haystack, string $needle);
 * Description: Finds the last occurrence of a character in a string.
 * Source code: ext/standard/string.c
*/

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

//multiple existance of needle in haystack
var_dump( strrchr("Hello, World", "o") );
var_dump( strrchr("Hello, World", "ooo") );

var_dump( strrchr("Hello, World", "Zzzz") ); //non-existant needle in haystack
echo "*** Done ***";
?>
--EXPECT--
*** Testing strrchr() function: basic functionality ***
unicode(12) "Hello, World"
unicode(12) "Hello, World"
unicode(12) "Hello, World"
unicode(12) "Hello, World"
bool(false)
bool(false)
unicode(5) "World"
unicode(5) "World"
unicode(7) ", World"
unicode(7) ", World"
unicode(12) "Hello, World"
unicode(12) "Hello, World"
unicode(4) "orld"
unicode(4) "orld"
bool(false)
*** Done ***
