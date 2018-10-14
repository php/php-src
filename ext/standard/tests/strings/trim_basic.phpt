--TEST--
Test trim() function : basic functionality
--FILE--
<?php

/* Prototype  : string trim  ( string $str  [, string $charlist  ] )
 * Description: Strip whitespace (or other characters) from the beginning and end of a string.
 * Source code: ext/standard/string.c
*/

echo "*** Testing trim() : basic functionality ***\n";

$text  = "  \t\r\n\0\x0B  ---These are a few words---  \t\r\n\0\x0B  ";
$hello  = "!===Hello World===!";
$binary = "\x0A\x0DExample string\x0A\x0D";

echo "\n-- Trim string with all white space characters --\n";
var_dump(trim($text));

echo "\n-- Trim non-whitespace from a string --\n";
var_dump(trim($hello, "=!"));

echo "\n-- Trim some non-white space characters from a string --\n";
var_dump(trim($hello, "Hdle"));

echo "\n-- Trim the ASCII control characters at the beginning of a string --\n";
var_dump(trim($binary, "\x00..\x1F"));

?>
===DONE===
--EXPECT--
*** Testing trim() : basic functionality ***

-- Trim string with all white space characters --
string(27) "---These are a few words---"

-- Trim non-whitespace from a string --
string(11) "Hello World"

-- Trim some non-white space characters from a string --
string(19) "!===Hello World===!"

-- Trim the ASCII control characters at the beginning of a string --
string(14) "Example string"
===DONE===
