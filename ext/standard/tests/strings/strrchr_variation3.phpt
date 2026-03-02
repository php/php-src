--TEST--
Test strrchr() function : usage variations - multi line heredoc string for 'haystack'
--FILE--
<?php
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
  $multi_line_str //needle as haystack
);

//loop through to test strrchr() with each needle
foreach($needles as $needle) {
  var_dump( strrchr($multi_line_str, $needle) );
}

echo "*** Done ***";
?>
--EXPECT--
*** Testing strrchr() function: with heredoc strings ***
string(19) "ing heredoc syntax."
bool(false)
string(8) " syntax."
string(63) "Example of string
spanning multiple lines
using heredoc syntax."
*** Done ***
