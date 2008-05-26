--TEST--
Bug #43541 (length parameter omitted or not does not work when casted to float)
--FILE--
<?php
$arr = array(1, 2, 3, 4, 5, 6);

var_dump(array_slice($arr, 0, (float)2));
var_dump(array_slice($arr, 0, (int)2));
?>
--EXPECT--
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
