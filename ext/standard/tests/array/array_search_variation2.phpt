--TEST--
Test array_search() function : usage variations - different haystack values
--FILE--
<?php
/* Test array_search() with different possible haystack values */

echo "*** Testing array_search() with different haystack values ***\n";

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
   $array_type on elements in $misc_array using array_search();
   checking PHP type comparison tables
*/
$counter = 1;
foreach($array_type as $type) {
  echo "-- Iteration $counter --\n";
  //loose type checking
  var_dump( array_search($type,$misc_array ) );
  //strict type checking
  var_dump( array_search($type,$misc_array,true) );
  //loose type checking
  var_dump( array_search($type,$misc_array,false) );
  $counter++;
}

echo "Done\n";
?>
--EXPECT--
*** Testing array_search() with different haystack values ***
-- Iteration 1 --
int(0)
int(3)
int(0)
-- Iteration 2 --
string(1) "y"
int(4)
string(1) "y"
-- Iteration 3 --
int(3)
bool(false)
int(3)
-- Iteration 4 --
string(1) "y"
int(2)
string(1) "y"
-- Iteration 5 --
int(3)
bool(false)
int(3)
-- Iteration 6 --
int(3)
bool(false)
int(3)
-- Iteration 7 --
int(2)
bool(false)
int(2)
-- Iteration 8 --
int(3)
bool(false)
int(3)
-- Iteration 9 --
string(1) "y"
string(1) "y"
string(1) "y"
-- Iteration 10 --
string(1) "y"
bool(false)
string(1) "y"
-- Iteration 11 --
int(3)
bool(false)
int(3)
-- Iteration 12 --
string(1) "y"
string(0) ""
string(1) "y"
Done
