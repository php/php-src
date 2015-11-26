--TEST--
Test array_unique() function : basic functionality 
--FILE--
<?php
/* Prototype  : array array_unique(array $input)
 * Description: Removes duplicate values from array 
 * Source code: ext/standard/array.c
*/

echo "*** Testing array_unique() : basic functionality ***\n";

// array with default keys
$input = array(1, 2, "1", '2');
var_dump( array_unique($input) );

// associative array
$input = array("1" => "one", 1 => "one", 2 => "two", '2' => "two");
var_dump( array_unique($input) );

// mixed array
$input = array("1" => "one", "two", "one", 2 => "two", "three");
var_dump( array_unique($input) );

echo "Done";
?>
--EXPECTF--
*** Testing array_unique() : basic functionality ***
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
array(2) {
  [1]=>
  string(3) "one"
  [2]=>
  string(3) "two"
}
array(3) {
  [1]=>
  string(3) "one"
  [2]=>
  string(3) "two"
  [4]=>
  string(5) "three"
}
Done
