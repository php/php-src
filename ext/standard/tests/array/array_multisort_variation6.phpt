--TEST--
Test array_multisort() function : usage variation - testing with multiple array arguments
--FILE--
<?php
/* Description: Sort multiple arrays at once similar to how ORDER BY clause works in SQL
 * Source code: ext/standard/array.c
 * Alias to functions:
 */

echo "*** Testing array_multisort() : Testing  all array sort specifiers ***\n";

$ar = array( 2, "aa" , "1");

array_multisort($ar, SORT_REGULAR, SORT_DESC);
var_dump($ar);

array_multisort($ar, SORT_STRING, SORT_DESC);
var_dump($ar);

array_multisort($ar, SORT_NUMERIC, SORT_DESC);
var_dump($ar);


?>
--EXPECT--
*** Testing array_multisort() : Testing  all array sort specifiers ***
array(3) {
  [0]=>
  int(2)
  [1]=>
  string(2) "aa"
  [2]=>
  string(1) "1"
}
array(3) {
  [0]=>
  string(2) "aa"
  [1]=>
  int(2)
  [2]=>
  string(1) "1"
}
array(3) {
  [0]=>
  int(2)
  [1]=>
  string(1) "1"
  [2]=>
  string(2) "aa"
}
