--TEST--
Test strripos() function : basic functionality - with default arguments
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

echo "\n-- regular string for haystack & needle --\n";
var_dump( strripos("Hello, World", "HeLLo") );
var_dump( strripos('Hello, World', "hello") );
var_dump( strripos("Hello, World", 'WoRLd') );
var_dump( strripos('Hello, World', 'WORLD') );
var_dump( strripos('Hello, World', 'foo') );

echo "\n-- single char for needle --\n";
var_dump( strripos("Hello, World", "O") );
var_dump( strripos("Hello, World", ",") );

echo "\n-- heredoc string for haystack & needle --\n";
var_dump( strripos($heredoc_str, "Hello, WoRLd") );
var_dump( strripos($heredoc_str, 'HelLO') );
var_dump( strripos($heredoc_str, $heredoc_str) );

?>
===DONE===
--EXPECT--
*** Testing strripos() function: basic functionality ***

-- regular string for haystack & needle --
int(0)
int(0)
int(7)
int(7)
bool(false)

-- single char for needle --
int(8)
int(5)

-- heredoc string for haystack & needle --
int(0)
int(0)
int(0)
===DONE===
