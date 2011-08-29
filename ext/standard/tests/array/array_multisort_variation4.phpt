--TEST--
Test array_multisort() function : usage variation - testing with multiple array arguments 
--FILE--
<?php
/* Prototype  : bool array_multisort(array ar1 [, SORT_ASC|SORT_DESC [, SORT_REGULAR|SORT_NUMERIC|SORT_STRING|SORT_NATURAL|SORT_FLAG_CASE]] [, array ar2 [, SORT_ASC|SORT_DESC [, SORT_REGULAR|SORT_NUMERIC|SORT_STRING|SORT_NATURAL|SORT_FLAG_CASE]], ...])
 * Description: Sort multiple arrays at once similar to how ORDER BY clause works in SQL 
 * Source code: ext/standard/array.c
 * Alias to functions: 
 */

echo "*** Testing array_multisort() : Testing  with multiple array arguments ***\n";

$arr1 = array (4,3,3,3);
$arr2 = array (9,3,2,2);
$arr3 = array (9,9,2,1);

var_dump(array_multisort($arr1, $arr2, $arr3));

var_dump($arr1);
var_dump($arr2);
var_dump($arr3);

?>
===DONE===
--EXPECTF--
*** Testing array_multisort() : Testing  with multiple array arguments ***
bool(true)
array(4) {
  [0]=>
  int(3)
  [1]=>
  int(3)
  [2]=>
  int(3)
  [3]=>
  int(4)
}
array(4) {
  [0]=>
  int(2)
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  int(9)
}
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(9)
  [3]=>
  int(9)
}
===DONE===
