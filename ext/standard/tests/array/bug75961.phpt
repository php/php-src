--TEST--
Bug #75961 (Strange references behavior)
--FILE--
<?php
$arr = [[1]];

array_walk($arr, function(){});
array_map('array_shift', $arr);
var_dump($arr);
?>
--EXPECT--
array(1) {
  [0]=>
  array(1) {
    [0]=>
    int(1)
  }
}
