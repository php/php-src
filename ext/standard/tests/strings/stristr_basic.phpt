--TEST--
Test stristr() function : basic functionality 
--FILE--
<?php
/* Prototype:  string stristr  ( string $haystack  , mixed $needle  [, bool $before_needle  ] )
   Description: Case-insensitive strstr().
*/

echo "*** Testing stristr() : basic functionality ***\n";

var_dump( stristr("Test string", "teSt") );
var_dump( stristr("test stRIng", "striNG") );
var_dump( stristr("teST StrinG", "stRIn") );
var_dump( stristr("tesT string", "t S") );
var_dump( stristr("test strinG", "g") );
var_dump( bin2hex(stristr(b"te".chr(0).b"St", chr(0))) );
var_dump( stristr("tEst", "test") );
var_dump( stristr("teSt", "test") );

var_dump( stristr("Test String", "String", false) );
var_dump( stristr("Test String", "String", true) );
?>
===DONE===
--EXPECTF--
*** Testing stristr() : basic functionality ***
string(11) "Test string"
string(6) "stRIng"
string(6) "StrinG"
string(8) "T string"
string(1) "G"
string(6) "005374"
string(4) "tEst"
string(4) "teSt"
string(6) "String"
string(5) "Test "
===DONE===
