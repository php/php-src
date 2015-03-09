--TEST--
Double array cast
--FILE--
<?php

$array = [1, 2, $x = 3];
var_dump((array) (array) $array);

?>
--EXPECT--
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
