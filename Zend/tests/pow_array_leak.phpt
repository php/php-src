--TEST--
Memory leak on ** with result==op1 array
--FILE--
<?php

$x = [0];
try {
    $x **= 1;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
var_dump($x);

$x = [0];
try {
    $x **= $x;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
var_dump($x);

?>
--EXPECT--
TypeError: Unsupported operand types: array ** int
array(1) {
  [0]=>
  int(0)
}
TypeError: Unsupported operand types: array ** array
array(1) {
  [0]=>
  int(0)
}
