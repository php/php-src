--TEST--
Test strrchr() function : usage variations - heredoc string containing escape sequences for 'haystack'
--FILE--
<?php
/* Prototype  : string strrchr(string $haystack, string $needle);
 * Description: Finds the last occurrence of a character in a string.
 * Source code: ext/standard/string.c
*/

/* Test strrchr() function by passing heredoc string containing 
 *  escape sequences for haystack and with various needles
*/

echo "*** Testing strrchr() function: with heredoc strings ***\n";
$escape_char_str = <<<EOD
\tes\t st\r\rch\r using
\escape \\seque\nce
EOD;

$needles = array(
  "\t",
  '\n',
  "\r",
  "\\",
  $escape_char_str //needle as haystack
);

//loop through to test strrchr() with each needle
foreach($needles as $needle) {
  var_dump( strrchr($escape_char_str, $needle) );
}

echo "*** Done ***";
?>
--EXPECTF--
*** Testing strrchr() function: with heredoc strings ***
string(33) "	 stch using
\escape \seque
ce"
string(9) "\seque
ce"
string(25) " using
\escape \seque
ce"
string(9) "\seque
ce"
string(33) "	 stch using
\escape \seque
ce"
*** Done ***
