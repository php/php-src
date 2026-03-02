--TEST--
Test wordwrap() function : usage variations  - valid break arguments(spaces)
--FILE--
<?php
/*
 *test wordwrap() with break arguments as single spaces
*/

echo "*** Testing wordwrap() : usage variations ***\n";

// Initialize all required variables
$str = "Testing wordrap function";
$width = 1;
$cut = false;

echo "\n-- Testing wordwrap() with default break value and single space as value --\n";
echo "-- with default break and cut value --\n";
var_dump( wordwrap($str, $width) );  // default break and cut value

echo "-- with default cut value --\n";
$break = ' ';
$break1 = "  ";
var_dump( wordwrap($str, $width, $break) );
var_dump( wordwrap($str, $width, $break1) );

echo "-- with cut value as false --\n";
$cut = false;
var_dump( wordwrap($str, $width, $break, $cut) );
var_dump( wordwrap($str, $width, $break1, $cut) );

echo "-- with cut value as true --\n";
$cut = true;
var_dump( wordwrap($str, $width, $break, $cut) );
var_dump( wordwrap($str, $width, $break1, $cut) );

echo "Done\n";
?>
--EXPECT--
*** Testing wordwrap() : usage variations ***

-- Testing wordwrap() with default break value and single space as value --
-- with default break and cut value --
string(24) "Testing
wordrap
function"
-- with default cut value --
string(24) "Testing wordrap function"
string(26) "Testing  wordrap  function"
-- with cut value as false --
string(24) "Testing wordrap function"
string(26) "Testing  wordrap  function"
-- with cut value as true --
string(43) "T e s t i n g w o r d r a p f u n c t i o n"
string(64) "T  e  s  t  i  n  g  w  o  r  d  r  a  p  f  u  n  c  t  i  o  n"
Done
