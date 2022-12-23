--TEST--
Test array_sum() function with objects that implement addition but not castable to numeric type
--EXTENSIONS--
zend_test
--FILE--
<?php
$input = [new DoOperationNoCast(25), new DoOperationNoCast(6)];
var_dump(array_sum($input));
?>
--EXPECTF--
Warning: Object of class DoOperationNoCast could not be converted to int|float in %s on line %d
int(0)
