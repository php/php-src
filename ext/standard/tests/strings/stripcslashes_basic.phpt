--TEST--
Test stripcslashes() function : basic functionality
--FILE--
<?php

echo "*** Testing stripcslashes() : basic functionality ***\n";
var_dump(stripcslashes('\H\e\l\l\o \W\or\l\d'));
var_dump(stripcslashes('Hello World\\r\\n'));
var_dump(stripcslashes('\\\Hello World'));
var_dump(stripcslashes('\x48\x65\x6c\x6c\x6f \x57\x6f\x72\x6c\x64'));
var_dump(stripcslashes('\110\145\154\154\157 \127\157\162\154\144'));

var_dump(bin2hex(stripcslashes('\\a')));
var_dump(bin2hex(stripcslashes('\\b')));
var_dump(bin2hex(stripcslashes('\\f')));
var_dump(bin2hex(stripcslashes('\\t')));
var_dump(bin2hex(stripcslashes('\\v')));
?>
--EXPECT--
*** Testing stripcslashes() : basic functionality ***
string(11) "Hello World"
string(13) "Hello World
"
string(12) "\Hello World"
string(11) "Hello World"
string(11) "Hello World"
string(2) "07"
string(2) "08"
string(2) "0c"
string(2) "09"
string(2) "0b"
