--TEST--
Test array_product() function with objects that implement addition but not castable to numeric type
--EXTENSIONS--
zend_test
--FILE--
<?php
$input = [new DoOperationNoCast(25), new DoOperationNoCast(6), new DoOperationNoCast(10)];

echo "array_product() version:\n";
var_dump(array_product($input));

echo "array_reduce() version:\n";
var_dump(array_reduce($input, fn($carry, $value) => $carry * $value, 1));
?>
--EXPECTF--
array_product() version:

Warning: array_product(): Multiplication is not supported on type DoOperationNoCast in %s on line %d

Warning: array_product(): Multiplication is not supported on type DoOperationNoCast in %s on line %d

Warning: array_product(): Multiplication is not supported on type DoOperationNoCast in %s on line %d
int(1)
array_reduce() version:
object(DoOperationNoCast)#5 (1) {
  ["val":"DoOperationNoCast":private]=>
  int(1500)
}
