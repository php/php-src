--TEST--
Test stripos() function : basic functionality - with default arguments
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

echo "-- With default arguments --\n";
//regular string for haystack & needle
var_dump( stripos("Hello, World", "Hello") );
var_dump( stripos('Hello, World', "hello") );
var_dump( stripos("Hello, World", 'World') );
var_dump( stripos('Hello, World', 'WORLD') );

//single char for needle
var_dump( stripos("Hello, World", "o") );
var_dump( stripos("Hello, World", ",") );

//heredoc string for haystack & needle
var_dump( stripos($heredoc_str, "Hello, World") );
var_dump( stripos($heredoc_str, 'Hello') );
var_dump( stripos($heredoc_str, $heredoc_str) );

//non-existing needle in haystack
var_dump( stripos("Hello, World", "ooo") );
echo "*** Done ***";
?>
--EXPECT--
*** Testing stripos() function: basic functionality ***
-- With default arguments --
int(0)
int(0)
int(7)
int(7)
int(4)
int(5)
int(0)
int(0)
int(0)
bool(false)
*** Done ***
