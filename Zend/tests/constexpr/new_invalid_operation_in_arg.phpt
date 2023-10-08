--TEST--
Invalid operation in new arg in const expr
--FILE--
<?php

$foo = [1, 2, 3];
static $x = new stdClass($foo);
var_dump($foo);

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
