--TEST--
Test strrpos() function : usage variations - heredoc string containing quotes for 'haystack' argument
--FILE--
<?php
/* Prototype  : int strrpos ( string $haystack, string $needle [, int $offset] );
 * Description: Find position of last occurrence of 'needle' in 'haystack'.
 * Source code: ext/standard/string.c
*/

/* Test strrpos() function by passing heredoc string containing quotes for haystack
 *  and with various needles & offsets
*/

echo "*** Testing strrpos() function: with heredoc strings ***\n";
echo "-- With heredoc string containing quote & slash chars --\n";
$quote_char_str = <<<EOD
it's bright,but i cann't see it.
"things in double quote"
'things in single quote'
this\line is /with\slashs
EOD;
var_dump( strrpos($quote_char_str, "line") );
var_dump( strrpos($quote_char_str, 'things') );
var_dump( strrpos($quote_char_str, 'things', 0) );
var_dump( strrpos($quote_char_str, "things", 20) );
echo "*** Done ***";
?>
--EXPECTF--
*** Testing strrpos() function: with heredoc strings ***
-- With heredoc string containing quote & slash chars --
int(88)
int(59)
int(59)
int(59)
*** Done ***
