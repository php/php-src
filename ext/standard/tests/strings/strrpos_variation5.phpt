--TEST--
Test strrpos() function : usage variations - heredoc string containing escape chars for 'haystack' argument
--FILE--
<?php
/* Prototype  : int strrpos ( string $haystack, string $needle [, int $offset] );
 * Description: Find position of last occurrence of 'needle' in 'haystack'.
 * Source code: ext/standard/string.c
*/

/* Test strrpos() function by passing heredoc string containing escape chars for haystack
 *  and with various needles & offsets
*/

echo "*** Testing strrpos() function: with heredoc strings ***\n";
echo "-- With heredoc string containing escape characters --\n";
$control_char_str = <<<EOD
Hello, World\n
Hello\tWorld
EOD;
var_dump( strrpos($control_char_str, "\n") );
var_dump( strrpos($control_char_str, "\t") );
var_dump( strrpos($control_char_str, "\n", 12) );
var_dump( strrpos($control_char_str, "\t", 15) );

echo "*** Done ***";
?>
--EXPECTF--
*** Testing strrpos() function: with heredoc strings ***
-- With heredoc string containing escape characters --
int(13)
int(19)
int(13)
int(19)
*** Done ***
