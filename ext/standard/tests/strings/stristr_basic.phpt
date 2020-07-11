--TEST--
Test stristr() function : basic functionality
--FILE--
<?php

echo "*** Testing stristr() : basic functionality ***\n";

var_dump( stristr("Test string", "teSt") );
var_dump( stristr("test stRIng", "striNG") );
var_dump( stristr("teST StrinG", "stRIn") );
var_dump( stristr("tesT string", "t S") );
var_dump( stristr("test strinG", "g") );
var_dump( bin2hex(stristr("te".chr(0)."St", chr(0))) );
var_dump( stristr("tEst", "test") );
var_dump( stristr("teSt", "test") );

var_dump( stristr("Test String", "String", false) );
var_dump( stristr("Test String", "String", true) );
?>
--EXPECT--
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
