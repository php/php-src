--TEST--
Test strripos() function : usage variations - heredoc string containing special chars for 'haystack' argument
--FILE--
<?php
/* Test strripos() function by passing heredoc string containing special chars for haystack
 *  and with various needles & offsets
*/

echo "*** Testing strripos() function: with heredoc strings ***\n";
echo "-- With heredoc string containing special chars --\n";
$special_chars_str = <<<EOD
Ex'ple of h'doc st'g, contains
$#%^*&*_("_")!#@@!$#$^^&$*(special)
chars.
EOD;
var_dump( strripos($special_chars_str, "Ex'pLE", 0) );
var_dump( strripos($special_chars_str, "!@@!", 23) );
var_dump( strripos($special_chars_str, '_') );
var_dump( strripos($special_chars_str, '("_")') );
var_dump( strripos($special_chars_str, "$*") );
var_dump( strripos($special_chars_str, "$*", 10) );
var_dump( strripos($special_chars_str, "(speCIal)") );

?>
--EXPECT--
*** Testing strripos() function: with heredoc strings ***
-- With heredoc string containing special chars --
int(0)
bool(false)
int(41)
int(39)
int(55)
int(55)
int(57)
