--TEST--
Test strrchr() function : usage variations - heredoc string containing blank line for 'haystack'
--FILE--
<?php
/* Test strrchr() function by passing heredoc string containing
 *  blank-line for haystack and with various needles
*/

echo "*** Testing strrchr() function: with heredoc strings ***\n";
$blank_line = <<<EOD

EOD;

$needles = array(
  "\n",
  '\n',
  "\r",
  "\r\n",
  "\t",
  "",
  $blank_line //needle as haystack
);

//loop through to test strrchr() with each needle
foreach($needles as $needle) {
  var_dump( strrchr($blank_line, $needle) );
}
echo "*** Done ***";
?>
--EXPECT--
*** Testing strrchr() function: with heredoc strings ***
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
*** Done ***
