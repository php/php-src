--TEST--
Test array_binary_search() function : usage variations - different needdle values
--FILE--
<?php
/*
 * Prototype  : bool array_binary_search ( mixed $needle, array $haystack [, bool $strict] )
 * Description: Used binary search to search haystack for needle and returns TRUE  
 *              if it is found in the array, FALSE otherwise.
 * Source Code: ext/standard/array.c
*/

/* Test array_binary_search with different possible needle values */

echo "*** Testing array_binary_search with different needle values ***\n";
$arrays = array (
  array("hello","world",6,"PHP","MYSQL","6","C","ZEND","binary","C++","perl","machine code",4),
);

$array_compare = array (
    4,
  "4",
  "6",
    6,
   10,
"PHP",
"asda"
);
/* loop to check if elements in $array_compare exist in $arrays
   using array_binary_search() */
$counter = 1;
foreach($arrays as $array) {
  foreach($array_compare as $compare) {
    sort($array);
    echo "-- Iteration $counter --\n";
    //strict option OFF
    var_dump(array_binary_search($compare,$array));  
    //strict option ON
    var_dump(array_binary_search($compare,$array,TRUE));  
    //strict option OFF
    var_dump(array_binary_search($compare,$array,FALSE));  
    $counter++;
 }
}
		
echo "Done\n";
?>
--EXPECTF--
*** Testing array_binary_search with different needle values ***
-- Iteration 1 --
bool(true)
bool(true)
bool(true)
-- Iteration 2 --
bool(true)
bool(false)
bool(true)
-- Iteration 3 --
bool(true)
bool(true)
bool(true)
-- Iteration 4 --
bool(true)
bool(true)
bool(true)
-- Iteration 5 --
bool(false)
bool(false)
bool(false)
-- Iteration 6 --
bool(true)
bool(true)
bool(true)
-- Iteration 7 --
bool(false)
bool(false)
bool(false)
Done

