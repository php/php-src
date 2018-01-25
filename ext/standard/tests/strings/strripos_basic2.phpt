--TEST--
Test strripos() function : basic functionality - with all arguments
--FILE--
<?php
/* Prototype  : int strripos ( string $haystack, string $needle [, int $offset] );
 * Description: Find position of last occurrence of a case-insensitive 'needle' in a 'haystack'
 * Source code: ext/standard/string.c
*/

echo "*** Testing strripos() function: basic functionality ***\n";
$heredoc_str = <<<EOD
Hello, World
EOD;

echo "\n-- regular string for haystack & needle, with various offsets --\n";
var_dump( strripos("Hello, World", "HeLLo", 0) );
var_dump( strripos("Hello, World", 'Hello', 1) );
var_dump( strripos('Hello, World', 'world', 1) );
var_dump( strripos('Hello, World', "WorLD", 5) );

echo "\n-- heredoc string for haystack & needle, with various offsets --\n";
var_dump( strripos($heredoc_str, "Hello, WORLD", 0) );
var_dump( strripos($heredoc_str, 'HelLo', 0) );
var_dump( strripos($heredoc_str, 'HeLLo', 1) );
var_dump( strripos($heredoc_str, $heredoc_str, 0) );
var_dump( strripos($heredoc_str, $heredoc_str, 1) );

echo "\n-- various +ve offsets --\n";
var_dump( strripos("Hello, World", "O", 3) );
var_dump( strripos("Hello, World", "O", 6) );
var_dump( strripos("Hello, World", "O", 10) );

echo "\n-- various -ve offsets --\n";
var_dump( strripos("Hello, World", "O", -1) );
var_dump( strripos("Hello, World", "O", -5) );
var_dump( strripos("Hello, World", "O",  -9) );
?>
===DONE===
--EXPECT--
*** Testing strripos() function: basic functionality ***

-- regular string for haystack & needle, with various offsets --
int(0)
bool(false)
int(7)
int(7)

-- heredoc string for haystack & needle, with various offsets --
int(0)
int(0)
bool(false)
int(0)
bool(false)

-- various +ve offsets --
int(8)
int(8)
bool(false)

-- various -ve offsets --
int(8)
int(4)
bool(false)
===DONE===
