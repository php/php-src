--TEST--
Test in_array() function : usage variations - different haystack values
--FILE--
<?php
/* Test in_array() with different possible haystack values */

echo "*** Testing in_array() with different haystack values ***\n";
$misc_array = array (
  'a',
  'key' =>'d',
  3,
  ".001" =>-67,
  "-.051" =>"k",
  0.091 =>"-.08",
  "e" =>"5",
  "y" =>NULL,
  NULL =>"",
  0,
  TRUE,
  FALSE,
  -27.39999999999,
  " ",
  "abcd\x00abcd\x00\abcd\x00abcdefghij",
  "abcd\nabcd\tabcd\rabcd\0abcd"
);
$array_type = array(TRUE, FALSE, 1, 0, -1, "1", "0", "-1", NULL, array(), "PHP", "");
/* loop to do loose and strict type check of elements in
   $array_type on elements in $misc_array using in_array();
   checking PHP type comparison tables
*/
$counter = 1;
foreach($array_type as $type) {
  echo "-- Iteration $counter --\n";
  //loose type checking
  var_dump( in_array($type,$misc_array ) );
  //strict type checking
  var_dump( in_array($type,$misc_array,true) );
  //loose type checking
  var_dump( in_array($type,$misc_array,false) );
  $counter++;
}

echo "Done\n";
?>
--EXPECT--
*** Testing in_array() with different haystack values ***
-- Iteration 1 --
bool(true)
bool(true)
bool(true)
-- Iteration 2 --
bool(true)
bool(true)
bool(true)
-- Iteration 3 --
bool(true)
bool(false)
bool(true)
-- Iteration 4 --
bool(true)
bool(true)
bool(true)
-- Iteration 5 --
bool(true)
bool(false)
bool(true)
-- Iteration 6 --
bool(true)
bool(false)
bool(true)
-- Iteration 7 --
bool(true)
bool(false)
bool(true)
-- Iteration 8 --
bool(true)
bool(false)
bool(true)
-- Iteration 9 --
bool(true)
bool(true)
bool(true)
-- Iteration 10 --
bool(true)
bool(false)
bool(true)
-- Iteration 11 --
bool(true)
bool(false)
bool(true)
-- Iteration 12 --
bool(true)
bool(true)
bool(true)
Done
