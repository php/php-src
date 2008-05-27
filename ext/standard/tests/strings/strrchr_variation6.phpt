--TEST--
Test strrchr() function : usage variations - heredoc string containing quote chars for 'haystack'
--FILE--
<?php
/* Prototype  : string strrchr(string $haystack, string $needle);
 * Description: Finds the last occurrence of a character in a string.
 * Source code: ext/standard/string.c
*/

/* Test strrchr() function by passing heredoc string containing quote chars for haystack
 *  and with various needles
*/

echo "*** Testing strrchr() function: with heredoc strings ***\n";
$quote_char_str = <<<EOD
"things" "in" "double" "quote"
'things' 'in' 'single' 'quote'
EOD;

$heredoc_needle = <<<EOD
quote
EOD;

$needles = array(
  "things",
  "\"things\"",
  "\'things\'",
  "in",
  "quote",
  $heredoc_needle, //needle as heredoc string
  $quote_char_str //needle as haystack
);

//loop through to test strrchr() with each needle
foreach($needles as $needle) {
  var_dump( strrchr($quote_char_str, $needle) );
}
echo "*** Done ***";
?>
--EXPECT--
*** Testing strrchr() function: with heredoc strings ***
unicode(3) "te'"
unicode(32) ""
'things' 'in' 'single' 'quote'"
bool(false)
unicode(14) "ingle' 'quote'"
unicode(6) "quote'"
unicode(6) "quote'"
unicode(32) ""
'things' 'in' 'single' 'quote'"
*** Done ***
