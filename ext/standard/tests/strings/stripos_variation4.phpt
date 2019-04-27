--TEST--
Test stripos() function : usage variations - heredoc string containing special chars for 'haystack' argument
--FILE--
<?php
/* Prototype  : int stripos ( string $haystack, string $needle [, int $offset] );
 * Description: Find position of first occurrence of a case-insensitive string
 * Source code: ext/standard/string.c
*/

/* Test stripos() function by passing heredoc string containing special chars for haystack
 *  and with various needles & offets
*/

echo "*** Testing stripos() function: with heredoc strings ***\n";
echo "-- With heredoc string containing special chars --\n";
$special_chars_str = <<<EOD
Ex'ple of h'doc st'g, contains
$#%^*&*_("_")!#@@!$#$^^&$*(special)
chars.
EOD;
var_dump( stripos($special_chars_str, "Ex'ple", 0) );
var_dump( stripos($special_chars_str, "!@@!", 23) );
var_dump( stripos($special_chars_str, '_') );
var_dump( stripos($special_chars_str, '("_")') );
var_dump( stripos($special_chars_str, "$*") );
var_dump( stripos($special_chars_str, "$*", 10) );
var_dump( stripos($special_chars_str, "(special)") );

echo "*** Done ***";
?>
--EXPECT--
*** Testing stripos() function: with heredoc strings ***
-- With heredoc string containing special chars --
int(0)
bool(false)
int(38)
int(39)
int(55)
int(55)
int(57)
*** Done ***
