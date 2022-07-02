--TEST--
Assigning the result of a non-reference function by-reference should not leak
--FILE--
<?php

function func() {
    return [0];
}

$x = $y =& func();
var_dump($x, $y);

?>
--EXPECTF--
Notice: Only variables should be assigned by reference in %s on line %d
array(1) {
  [0]=>
  int(0)
}
array(1) {
  [0]=>
  int(0)
}
