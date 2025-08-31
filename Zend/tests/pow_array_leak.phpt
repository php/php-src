--TEST--
Memory leak on ** with result==op1 array
--FILE--
<?php

$x = [0];
try {
    $x **= 1;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
var_dump($x);

$x = [0];
try {
    $x **= $x;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
var_dump($x);

?>
--EXPECT--
Unsupported operand types: array ** int
array(1) {
  [0]=>
  int(0)
}
Unsupported operand types: array ** array
array(1) {
  [0]=>
  int(0)
}
