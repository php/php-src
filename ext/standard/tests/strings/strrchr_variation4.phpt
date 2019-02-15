--TEST--
Test strrchr() function : usage variations - heredoc string containing special chars for 'haystack'
--FILE--
<?php
/* Prototype  : string strrchr(string $haystack, string $needle);
 * Description: Finds the last occurrence of a character in a string.
 * Source code: ext/standard/string.c
*/

/* Test strrchr() function by passing heredoc string containing special chars for haystack
 * and with various needles
*/

echo "*** Testing strrchr() function: with heredoc strings ***\n";
$special_chars_str = <<<EOD
Example of heredoc string contains
$#%^*&*_("_")!#@@!$#$^^&*(special)
chars.
EOD;

$heredoc_needle = <<<EOD
^^&*(
EOD;

$needles = array(
  "!@@!",
  '_',
  '("_")',
  "$*",
  "(special)",
  $heredoc_needle,  //needle as heredoc string
  $special_chars_str  //needle as haystack
);

//loop through to test strrchr() with each needle
foreach($needles as $needle) {
  var_dump( strrchr($special_chars_str, $needle) );
}
echo "*** Done ***";
?>
--EXPECT--
*** Testing strrchr() function: with heredoc strings ***
string(24) "!$#$^^&*(special)
chars."
string(31) "_")!#@@!$#$^^&*(special)
chars."
string(16) "(special)
chars."
string(21) "$^^&*(special)
chars."
string(16) "(special)
chars."
string(19) "^&*(special)
chars."
string(76) "Example of heredoc string contains
$#%^*&*_("_")!#@@!$#$^^&*(special)
chars."
*** Done ***
