--TEST--
Test natcasesort() function : usage variations - recursive arrays
--FILE--
<?php
/* Prototype  : bool natcasesort(array &$array_arg)
 * Description: Sort an array using case-insensitive natural sort
 * Source code: ext/standard/array.c
 */

/*
 * Pass natcasesort() an infinitely recursive array to test how it is re-ordered
 */

echo "*** Testing natcasesort() : usage variations ***\n";

$array = array (1, 3.00, 'zero', '2');
$array[] = &$array;
var_dump($array);

var_dump(@natcasesort($array));
var_dump($array);

echo "Done";
?>
--EXPECT--
*** Testing natcasesort() : usage variations ***
array(5) {
  [0]=>
  int(1)
  [1]=>
  float(3)
  [2]=>
  string(4) "zero"
  [3]=>
  string(1) "2"
  [4]=>
  &array(5) {
    [0]=>
    int(1)
    [1]=>
    float(3)
    [2]=>
    string(4) "zero"
    [3]=>
    string(1) "2"
    [4]=>
    *RECURSION*
  }
}
bool(true)
array(5) {
  [0]=>
  int(1)
  [3]=>
  string(1) "2"
  [1]=>
  float(3)
  [4]=>
  &array(5) {
    [0]=>
    int(1)
    [3]=>
    string(1) "2"
    [1]=>
    float(3)
    [4]=>
    *RECURSION*
    [2]=>
    string(4) "zero"
  }
  [2]=>
  string(4) "zero"
}
Done
