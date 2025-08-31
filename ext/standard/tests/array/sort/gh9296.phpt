--TEST--
GH-9296: incorrect ksort(..., SORT_REGULAR) behaviour on arrays with numeric and string keys
--FILE--
<?php
$array = ["aaa" => 0, 600 => 1];
ksort($array, SORT_REGULAR);
var_dump($array);
var_dump(array_key_first($array) <=> array_key_last($array));
?>
--EXPECT--
array(2) {
  [600]=>
  int(1)
  ["aaa"]=>
  int(0)
}
int(-1)
