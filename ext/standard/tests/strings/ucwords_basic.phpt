--TEST--
Test ucwords() function : basic functionality 
--FILE--
<?php
/* Prototype  : string ucwords ( string $str )
 * Description: Uppercase the first character of each word in a string
 * Source code: ext/standard/string.c
*/

echo "*** Testing ucwords() : basic functionality ***\n";

// lines with different whitespace charecter
$str_array = array(
 "testing ucwords",
 'testing ucwords',
 'testing\tucwords',
 "testing\tucwords",
 "testing\nucwords",
 'testing\nucwords',
 "testing\vucwords",
 'testing\vucwords',
 "testing",
 'testing',
 ' testing',
 " testing",
 "testing  ucwords",
 'testing  ucwords',
 'testing\rucwords',
 "testing\rucwords",
 'testing\fucwords',
 "testing\fucwords"
);

// loop through the $strings array to test ucwords on each element 
$iteration = 1;
for($index = 0; $index < count($str_array); $index++) {
  echo "-- Iteration $iteration --\n";
  var_dump( ucwords($str_array[$index]) );
  $iteration++;
}

echo "Done\n";
?>
--EXPECT--
*** Testing ucwords() : basic functionality ***
-- Iteration 1 --
unicode(15) "Testing Ucwords"
-- Iteration 2 --
unicode(15) "Testing Ucwords"
-- Iteration 3 --
unicode(16) "Testing\tucwords"
-- Iteration 4 --
unicode(15) "Testing	Ucwords"
-- Iteration 5 --
unicode(15) "Testing
Ucwords"
-- Iteration 6 --
unicode(16) "Testing\nucwords"
-- Iteration 7 --
unicode(15) "TestingUcwords"
-- Iteration 8 --
unicode(16) "Testing\vucwords"
-- Iteration 9 --
unicode(7) "Testing"
-- Iteration 10 --
unicode(7) "Testing"
-- Iteration 11 --
unicode(8) " Testing"
-- Iteration 12 --
unicode(8) " Testing"
-- Iteration 13 --
unicode(16) "Testing  Ucwords"
-- Iteration 14 --
unicode(16) "Testing  Ucwords"
-- Iteration 15 --
unicode(16) "Testing\rucwords"
-- Iteration 16 --
unicode(15) "TestingUcwords"
-- Iteration 17 --
unicode(16) "Testing\fucwords"
-- Iteration 18 --
unicode(15) "TestingUcwords"
Done
