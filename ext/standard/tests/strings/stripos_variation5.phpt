--TEST--
Test stripos() function : usage variations - heredoc string containing escape chars for 'haystack' argument
--FILE--
<?php
/* Prototype  : int stripos ( string $haystack, string $needle [, int $offset] );
 * Description: Find position of first occurrence of a case-insensitive string
 * Source code: ext/standard/string.c
*/

/* Test stripos() function by passing heredoc string containing escape chars for haystack 
 *  and with various needles & offsets 
*/

echo "*** Testing stripos() function: with heredoc strings ***\n";
echo "-- With heredoc string containing escape characters --\n";
$control_char_str = <<<EOD
Hello, World\n
Hello\tWorld
EOD;
var_dump( stripos($control_char_str, "\n") );
var_dump( stripos($control_char_str, "\t") );
var_dump( stripos($control_char_str, "\n", 12) );
var_dump( stripos($control_char_str, "\t", 15) );

echo "*** Done ***";
?>
--EXPECTF--
*** Testing stripos() function: with heredoc strings ***
-- With heredoc string containing escape characters --
int(12)
int(19)
int(12)
int(19)
*** Done ***
