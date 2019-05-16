--TEST--
Test stripos() function : basic functionality - with all arguments
--FILE--
<?php
/* Prototype  : int stripos ( string $haystack, string $needle [, int $offset] );
 * Description: Find position of first occurrence of a case-insensitive string
 * Source code: ext/standard/string.c
*/

echo "*** Testing stripos() function: basic functionality ***\n";
$heredoc_str = <<<Identifier
Hello, World
Identifier;

echo "-- With all arguments --\n";
//regular string for haystack & needle, with various offsets
var_dump( stripos("Hello, World", "Hello", 0) );
var_dump( stripos("Hello, World", 'Hello', 1) );
var_dump( stripos('Hello, World', 'WORLD', 1) );
var_dump( stripos('Hello, World', "WoRld", 5) );
var_dump( stripos('Hello, World', "WoRld", -6) );
var_dump( stripos('Hello, World', "WoRld", -3) );
var_dump( stripos('Hello, World', "WoRld", -12) );

//heredoc string for haystack & needle, with various offsets
var_dump( stripos($heredoc_str, "Hello, World", 0) );
var_dump( stripos($heredoc_str, 'Hello', 0) );
var_dump( stripos($heredoc_str, 'Hello', 1) );
var_dump( stripos($heredoc_str, $heredoc_str, 0) );
var_dump( stripos($heredoc_str, $heredoc_str, 1) );
var_dump( stripos($heredoc_str, $heredoc_str, -strlen($heredoc_str)) );
var_dump( stripos($heredoc_str, $heredoc_str, -strlen($heredoc_str)+1) );

//various offsets
var_dump( stripos("Hello, World", "o", 3) );
var_dump( stripos("Hello, World", "O", 5) );
var_dump( stripos("Hello, World", "o", 6) );
var_dump( stripos("Hello, World", "o", 10) );
var_dump( stripos("Hello, World", "o", -7) );
var_dump( stripos("Hello, World", "o", -8) );
var_dump( stripos("Hello, World", "o", -10) );
var_dump( stripos("Hello, World", "o", -4) );
var_dump( stripos("Hello, World", "o", -3) );
echo "*** Done ***";
?>
--EXPECT--
*** Testing stripos() function: basic functionality ***
-- With all arguments --
int(0)
bool(false)
int(7)
int(7)
int(7)
bool(false)
int(7)
int(0)
int(0)
bool(false)
int(0)
bool(false)
int(0)
bool(false)
int(4)
int(8)
int(8)
bool(false)
int(8)
int(4)
int(4)
int(8)
bool(false)
*** Done ***
