--TEST--
Test rtrim() function : basic functionality
--FILE--
<?php

echo "*** Testing rtrim() : basic functionality ***\n";

$text  = "---These are a few words---  \t\r\n\0\x0B  ";
$hello  = "!===Hello World===!";
$alpha = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
$binary = "Example string\x0A\x0D";



echo "\n-- Trim string with all white space characters --\n";
var_dump(rtrim($text));

echo "\n-- Trim non-whitespace from a string --\n";
var_dump(rtrim($hello, "=!"));

echo "\n-- Trim some non-white space characters from a string --\n";
var_dump(rtrim($hello, "!dlWro="));

echo "\n-- Trim some non-white space characters from a string using a character range --\n";
var_dump(rtrim($alpha, "A..Z"));

echo "\n-- Trim the ASCII control characters at the beginning of a string --\n";
var_dump(rtrim($binary, "\x00..\x1F"));

?>
--EXPECT--
*** Testing rtrim() : basic functionality ***

-- Trim string with all white space characters --
string(27) "---These are a few words---"

-- Trim non-whitespace from a string --
string(15) "!===Hello World"

-- Trim some non-white space characters from a string --
string(10) "!===Hello "

-- Trim some non-white space characters from a string using a character range --
string(10) "0123456789"

-- Trim the ASCII control characters at the beginning of a string --
string(14) "Example string"
