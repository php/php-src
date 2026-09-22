--TEST--
GH-11320: Array literals can contain list() assignments
--FILE--
<?php
$index = 1;
function getList() { return [2, 3]; }
var_dump([$index => list($x, $y) = getList()]);
var_dump([$index => [$x, $y] = getList()]);
?>
--EXPECT--
array(1) {
  [1]=>
  array(2) {
    [0]=>
    int(2)
    [1]=>
    int(3)
  }
}
array(1) {
  [1]=>
  array(2) {
    [0]=>
    int(2)
    [1]=>
    int(3)
  }
}
