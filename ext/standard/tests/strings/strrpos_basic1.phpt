--TEST--
Test strrpos() function : basic functionality - with default arguments
--FILE--
<?php
/* Prototype  : int strrpos ( string $haystack, string $needle [, int $offset] );
 * Description: Find position of last occurrence of 'needle' in 'haystack'
 * Source code: ext/standard/string.c
*/

echo "*** Testing strrpos() function: basic functionality ***\n";
$heredoc_str = <<<EOD
Hello, World
EOD;

echo "-- With default arguments --\n";
//regular string for haystack & needle
var_dump( strrpos("Hello, World", "Hello") );
var_dump( strrpos('Hello, World', "hello") );
var_dump( strrpos("Hello, World", 'World') );
var_dump( strrpos('Hello, World', 'WORLD') );

//single char for needle
var_dump( strrpos("Hello, World", "o") );
var_dump( strrpos("Hello, World", ",") );

//heredoc string for haystack & needle
var_dump( strrpos($heredoc_str, "Hello, World") );
var_dump( strrpos($heredoc_str, 'Hello') );
var_dump( strrpos($heredoc_str, $heredoc_str) );

echo "*** Done ***";
?>
--EXPECTF--
*** Testing strrpos() function: basic functionality ***
-- With default arguments --
int(0)
bool(false)
int(7)
bool(false)
int(8)
int(5)
int(0)
int(0)
int(0)
*** Done ***
