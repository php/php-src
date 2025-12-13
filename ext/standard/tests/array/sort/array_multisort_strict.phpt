--TEST--
Test array_multisort() function : strict type sorting
--FILE--
<?php
echo "*** Testing array_multisort() : strict type sorting\n";

$arr1 = [1, "1", 2, "2"];
$arr2 = ["a", "b", "c", "d"];

array_multisort($arr1, SORT_STRICT, $arr2);

var_dump($arr1, $arr2);

echo "\n*** Testing array_multisort() : strict type sorting descending\n";

$arr1 = [1, "1", 2, "2"];
$arr2 = ["a", "b", "c", "d"];

array_multisort($arr1, SORT_DESC, SORT_STRICT, $arr2);

var_dump($arr1, $arr2);

?>
--EXPECT--
*** Testing array_multisort() : strict type sorting
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  string(1) "1"
  [3]=>
  string(1) "2"
}
array(4) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "c"
  [2]=>
  string(1) "b"
  [3]=>
  string(1) "d"
}

*** Testing array_multisort() : strict type sorting descending
array(4) {
  [0]=>
  string(1) "2"
  [1]=>
  string(1) "1"
  [2]=>
  int(2)
  [3]=>
  int(1)
}
array(4) {
  [0]=>
  string(1) "d"
  [1]=>
  string(1) "b"
  [2]=>
  string(1) "c"
  [3]=>
  string(1) "a"
}
