--TEST--
Test strrpos() function : usage variations - multi line heredoc string for 'haystack' argument
--FILE--
<?php
/* Prototype  : int strrpos ( string $haystack, string $needle [, int $offset] );
 * Description: Find position of last occurrence of 'needle' in 'haystack'.
 * Source code: ext/standard/string.c
*/

/* Test strrpos() function by passing multi-line heredoc string for haystack and
 *  with various needles & offsets
*/

echo "*** Testing strrpos() function: with heredoc strings ***\n";
echo "-- With heredoc string containing multi lines --\n";
$multi_line_str = <<<EOD
Example of string
spanning multiple lines
using heredoc syntax.
EOD;
var_dump( strrpos($multi_line_str, "ing", 0) );
var_dump( strrpos($multi_line_str, "ing", 15) );
var_dump( strrpos($multi_line_str, "ing", 22) );
var_dump( strrpos($multi_line_str, "") );
var_dump( strrpos($multi_line_str, " ") );

echo "*** Done ***";
?>
--EXPECTF--
*** Testing strrpos() function: with heredoc strings ***
-- With heredoc string containing multi lines --
int(44)
int(44)
int(44)
bool(false)
int(55)
*** Done ***
