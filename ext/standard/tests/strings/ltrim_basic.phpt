--TEST--
Test ltrim() function : basic functionality
--FILE--
<?php

/* Prototype  : string ltrim  ( string $str  [, string $charlist  ] )
 * Description: Strip whitespace (or other characters) from the beginning of a string.
 * Source code: ext/standard/string.c
*/

echo "*** Testing ltrim() : basic functionality ***\n";

$text  = " \t\r\n\0\x0B ---These are a few words---  ";
$hello  = "!===Hello World===!";
$binary = "\x09\x0AExample string";
$alpha = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

echo "\n-- Trim string with all white space characters --\n";
var_dump(ltrim($text));

echo "\n-- Trim non-whitespace from a string --\n";
var_dump(ltrim($hello, "=!"));

echo "\n-- Trim some non-white space characters from a string --\n";
var_dump(ltrim($hello, "!oleH="));

echo "\n-- Trim some non-white space characters from a string suing a character range --\n";
var_dump(ltrim($alpha, "A..Z"));


echo "\n-- Trim the ASCII control characters at the beginning of a string --\n";
var_dump(ltrim($binary, "\x00..\x1F"));

?>
===DONE===
--EXPECT--
*** Testing ltrim() : basic functionality ***

-- Trim string with all white space characters --
string(29) "---These are a few words---  "

-- Trim non-whitespace from a string --
string(15) "Hello World===!"

-- Trim some non-white space characters from a string --
string(10) " World===!"

-- Trim some non-white space characters from a string suing a character range --
string(10) "0123456789"

-- Trim the ASCII control characters at the beginning of a string --
string(14) "Example string"
===DONE===
