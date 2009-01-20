--TEST--
Test stripcslashes() function : basic functionality 
--FILE--
<?php

/* Prototype  : string stripcslashes  ( string $str  )
 * Description: Returns a string with backslashes stripped off. Recognizes C-like \n, \r ..., 
 *              octal and hexadecimal representation.
 * Source code: ext/standard/string.c
*/

echo "*** Testing stripcslashes() : basic functionality ***\n";
var_dump(stripcslashes('\H\e\l\l\o \W\or\l\d'));
var_dump(stripcslashes('Hello World\\r\\n'));
var_dump(stripcslashes('\x48\x65\x6c\x6c\x6f \x57\x6f\x72\x6c\x64'));
var_dump(stripcslashes('\110\145\154\154\157 \127\157\162\154\144'));

?>
===DONE===
--EXPECT--
*** Testing stripcslashes() : basic functionality ***
string(11) "Hello World"
string(13) "Hello World
"
string(11) "Hello World"
string(11) "Hello World"
===DONE===

