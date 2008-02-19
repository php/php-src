--TEST--
Test natcasesort() function : usage variations - mixed array
--FILE--
<?php
/* Prototype  : bool natcasesort(array &$array_arg)
 * Description: Sort an array using case-insensitive natural sort
 * Source code: ext/standard/array.c
 */

/*
 * Pass an array containing sub-arrays, ints, floats, strings, boolean, null 
 * and escape characters to test how natcasesort() re-orders it
 */

echo "*** Testing natcasesort() : usage variation ***\n";

$mixed_values = array (
  array(), 
  array( array(33, -5, 6), 
         array(11), 
         array(22, -55), 
         array() 
       ),
  -4, "4", 4.00, "b", "5", -2, -2.0, -2.98989, "-.9", "True", "",
  NULL, "ab", "abcd", 0.0, -0, "abcd\x00abcd\x00abcd", '', true, false
);
// suppress errors as is generating a lot of "array to string" notices
var_dump( @natcasesort($mixed_values) );

var_dump($mixed_values);

echo "Done";
?>

--EXPECTF--
*** Testing natcasesort() : usage variation ***
bool(true)
array(22) {
  [13]=>
  NULL
  [19]=>
  string(0) ""
  [21]=>
  bool(false)
  [12]=>
  string(0) ""
  [10]=>
  string(3) "-.9"
  [7]=>
  int(-2)
  [8]=>
  float(-2)
  [9]=>
  float(-2.98989)
  [2]=>
  int(-4)
  [16]=>
  float(0)
  [17]=>
  int(0)
  [20]=>
  bool(true)
  [3]=>
  string(1) "4"
  [4]=>
  float(4)
  [6]=>
  string(1) "5"
  [14]=>
  string(2) "ab"
  [15]=>
  string(4) "abcd"
  [18]=>
  string(14) "%s"
  [0]=>
  array(0) {
  }
  [1]=>
  array(4) {
    [0]=>
    array(3) {
      [0]=>
      int(33)
      [1]=>
      int(-5)
      [2]=>
      int(6)
    }
    [1]=>
    array(1) {
      [0]=>
      int(11)
    }
    [2]=>
    array(2) {
      [0]=>
      int(22)
      [1]=>
      int(-55)
    }
    [3]=>
    array(0) {
    }
  }
  [5]=>
  string(1) "b"
  [11]=>
  string(4) "True"
}
Done