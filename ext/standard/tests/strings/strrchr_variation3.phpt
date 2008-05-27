--TEST--
Test strrchr() function : usage variations - multi line heredoc string for 'haystack'
--FILE--
<?php
/* Prototype  : string strrchr(string $haystack, string $needle);
 * Description: Finds the last occurrence of a character in a string.
 * Source code: ext/standard/string.c
*/

/* Test strrchr() function by passing multi-line heredoc string for haystack and
 *    with various needles
*/

echo "*** Testing strrchr() function: with heredoc strings ***\n";
$multi_line_str = <<<EOD
Example of string
spanning multiple lines
using heredoc syntax.
EOD;

$needles = array(
  "ing", 
  "", 
  " ",
  $multi_line_str //needle as heredoc string
);

//loop through to test strrchr() with each needle
foreach($needles as $needle) {  
  var_dump( strrchr($multi_line_str, $needle) );
}

echo "*** Done ***";
?>
--EXPECT--
*** Testing strrchr() function: with heredoc strings ***
unicode(19) "ing heredoc syntax."
bool(false)
unicode(8) " syntax."
unicode(63) "Example of string
spanning multiple lines
using heredoc syntax."
*** Done ***
