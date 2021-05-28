--TEST--
Test addcslashes() function (variation 3)
--INI--
precision=14
--FILE--
<?php

/* Miscellaneous input */
echo "\n*** Testing addcslashes() with miscellaneous input arguments ***\n";
var_dump( addcslashes("", "") );
var_dump( addcslashes("", "burp") );
var_dump( addcslashes("kaboemkara!", "") );
var_dump( addcslashes("foobarbaz", 'bar') );
var_dump( addcslashes('foo[ ]', 'A..z') );
var_dump( @addcslashes("zoo['.']", 'z..A') );
var_dump( addcslashes('abcdefghijklmnopqrstuvwxyz', "a\145..\160z") );
var_dump( addcslashes( 123, 123 ) );
var_dump( addcslashes( 0, 0 ) );
var_dump( addcslashes( "\0" , 0 ) );
var_dump( addcslashes( -1.234578, 3 ) );
var_dump( addcslashes( " ", " ") );
var_dump( addcslashes( "string\x00with\x00NULL", "\0") );

echo "Done\n";

?>
--EXPECTF--
*** Testing addcslashes() with miscellaneous input arguments ***
string(0) ""
string(0) ""
string(11) "kaboemkara!"
string(14) "foo\b\a\r\b\az"
string(11) "\f\o\o\[ \]"
string(10) "\zoo['\.']"
string(40) "\abcd\e\f\g\h\i\j\k\l\m\n\o\pqrstuvwxy\z"
string(6) "\1\2\3"
string(2) "\0"
string(1) "%0"
string(10) "-1.2\34578"
string(2) "\ "
string(22) "string\000with\000NULL"
Done
