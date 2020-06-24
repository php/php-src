--TEST--
Test strrchr() function : usage variations - empty heredoc string for 'haystack'
--FILE--
<?php
/* Test strrchr() function by passing empty heredoc string for haystack
 *  and with various needles
*/

echo "*** Testing strrchr() function: with heredoc strings ***\n";
$empty_str = <<<EOD
EOD;

$needles = array(
  "",
  '',
  FALSE,
  NULL,
  "\0",
  $empty_str //needle as haystack
);

//loop through to test strrchr() with each needle
foreach($needles as $needle) {
  var_dump( strrchr($empty_str, $needle) );
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
*** Done ***
