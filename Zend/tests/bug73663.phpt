--TEST--
Bug #73663 ("Invalid opcode 65/16/8" occurs with a variable created with list())
--FILE--
<?php
function change(&$ref) {
    $ref = range(1, 10);
    return;
}

$func = function (&$ref) {
    return change($ref);
};

$array = [1];
var_dump(list($val) = $array); // NG: Invalid opcode

change(list(&$val) = $array);
var_dump($array);

$array = [1];
$func(list(&$val) = $array);
var_dump($array);
?>
--EXPECT--
array(1) {
  [0]=>
  int(1)
}
array(10) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  int(4)
  [4]=>
  int(5)
  [5]=>
  int(6)
  [6]=>
  int(7)
  [7]=>
  int(8)
  [8]=>
  int(9)
  [9]=>
  int(10)
}
array(10) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  int(4)
  [4]=>
  int(5)
  [5]=>
  int(6)
  [6]=>
  int(7)
  [7]=>
  int(8)
  [8]=>
  int(9)
  [9]=>
  int(10)
}
