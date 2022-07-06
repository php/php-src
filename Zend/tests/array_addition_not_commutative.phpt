--TEST--
Array addition is not commutative -- do not swap operands
--FILE--
<?php

$array = [1, 2, 3];
$array = [4, 5, 6] + $array;
var_dump($array);

?>
--EXPECT--
array(3) {
  [0]=>
  int(4)
  [1]=>
  int(5)
  [2]=>
  int(6)
}
