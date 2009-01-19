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
var_dump( bin2hex((binary)stristr(b"te".chr(0).b"St", chr(0))) );
var_dump( stristr("tEst", "test") );
var_dump( stristr("teSt", "test") );

var_dump( stristr("Test String", "String", false) );
var_dump( stristr("Test String", "String", true) );
?>
===DONE===
--EXPECTF--
*** Testing stristr() : basic functionality ***
unicode(11) "Test string"
unicode(6) "stRIng"
unicode(6) "StrinG"
unicode(8) "T string"
unicode(1) "G"
unicode(6) "005374"
unicode(4) "tEst"
unicode(4) "teSt"
unicode(6) "String"
unicode(5) "Test "
===DONE===
