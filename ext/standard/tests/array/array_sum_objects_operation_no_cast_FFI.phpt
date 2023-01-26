--TEST--
Test array_sum() function with objects that implement addition but not castable to numeric type
--EXTENSIONS--
ffi
--FILE--
<?php

$x = FFI::new("int[2]");
$x[0] = 10;
$x[1] = 25;

$input = [$x, 1];

echo "array_sum() version:\n";
var_dump(array_sum($input));

echo "array_reduce() version:\n";
var_dump(array_reduce($input, fn($carry, $value) => $carry + $value, 0));
?>
--EXPECTF--
array_sum() version:

Warning: Object of class FFI\CData could not be converted to int|float in %s on line %d
int(0)
array_reduce() version:
object(FFI\CData:int32_t*)#4 (1) {
  [0]=>
  int(25)
}
