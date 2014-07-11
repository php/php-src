--TEST--
Array offsets can be yielded by reference
--FILE--
<?php

function &gen(array &$array) {
    yield $array[0];
}

$array = [1, 2, 3];
$gen = gen($array);
foreach ($gen as &$val) {
    $val *= -1;
}
var_dump($array);

?>
--EXPECT--
array(3) {
  [0]=>
  &int(-1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
