--TEST--
Test array_sum() function with objects that implement addition but not castable to numeric type
--EXTENSIONS--
zend_test
--FILE--
<?php
$input = [new DoOperationNoCast(25), new DoOperationNoCast(6)];

echo "array_sum() version:\n";
var_dump(array_sum($input));

echo "array_reduce() version:\n";
var_dump(array_reduce($input, fn($carry, $value) => $carry + $value, 0));
?>
--EXPECTF--
array_sum() version:

Warning: Object of class DoOperationNoCast could not be converted to int|float in %s on line %d
int(0)
array_reduce() version:
object(DoOperationNoCast)#5 (1) {
  ["val":"DoOperationNoCast":private]=>
  int(31)
}
