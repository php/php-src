--TEST--
Test strrpos() function : usage variations - heredoc string containing special chars for 'haystack' argument
--FILE--
<?php
/* Prototype  : int strrpos ( string $haystack, string $needle [, int $offset] );
 * Description: Find position of last occurrence of 'needle' in 'haystack'.
 * Source code: ext/standard/string.c
*/

/* Test strrpos() function by passing heredoc string containing special chars for haystack
 *  and with various needles & offets
*/

echo "*** Testing strrpos() function: with heredoc strings ***\n";
echo "-- With heredoc string containing special chars --\n";
$special_chars_str = <<<EOD
Ex'ple of h'doc st'g, contains
$#%^*&*_("_")!#@@!$#$^^&$*(special)
chars.
EOD;
var_dump( strrpos($special_chars_str, "Ex'ple", 0) );
var_dump( strrpos($special_chars_str, "!@@!", 23) );
var_dump( strrpos($special_chars_str, '_') );
var_dump( strrpos($special_chars_str, '("_")') );
var_dump( strrpos($special_chars_str, "$*") );
var_dump( strrpos($special_chars_str, "$*", 10) );
var_dump( strrpos($special_chars_str, "(special)") );

echo "*** Done ***";
?>
--EXPECTF--
*** Testing strrpos() function: with heredoc strings ***
-- With heredoc string containing special chars --
int(0)
bool(false)
int(41)
int(39)
int(55)
int(55)
int(57)
*** Done ***
