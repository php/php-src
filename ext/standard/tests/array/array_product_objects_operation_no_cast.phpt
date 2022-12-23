--TEST--
Test array_product() function with objects that implement addition but not castable to numeric type
--EXTENSIONS--
zend_test
--FILE--
<?php
$input = [new DoOperationNoCast(25), new DoOperationNoCast(6), new DoOperationNoCast(10)];
var_dump(array_product($input));
?>
--EXPECTF--
Warning: Object of class DoOperationNoCast could not be converted to int|float in %s on line %d
int(1)
