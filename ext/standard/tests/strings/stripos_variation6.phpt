--TEST--
Test stripos() function : usage variations - heredoc string containing quotes for 'haystack' argument
--FILE--
<?php
/* Test stripos() function by passing heredoc string containing quotes for haystack
 *  and with various needles & offsets
*/

echo "*** Testing stripos() function: with heredoc strings ***\n";
echo "-- With heredoc string containing quote & slash chars --\n";
$quote_char_str = <<<EOD
it's bright,but i cann't see it.
"things in double quote"
'things in single quote'
this\line is /with\slashs
EOD;
var_dump( stripos($quote_char_str, "line") );
var_dump( stripos($quote_char_str, 'things') );
var_dump( stripos($quote_char_str, 'things', 0) );
var_dump( stripos($quote_char_str, "things", 20) );
echo "*** Done ***";
?>
--EXPECT--
*** Testing stripos() function: with heredoc strings ***
-- With heredoc string containing quote & slash chars --
int(88)
int(34)
int(34)
int(34)
*** Done ***
