--TEST--
Test DoOperationNotCast dummy class
--EXTENSIONS--
zend_test
--FILE--
<?php

$a = new DoOperationNoCast(25);
$b = new DoOperationNoCast(6);

var_dump($a + $b);
var_dump($a * $b);

?>
--EXPECT--
object(DoOperationNoCast)#3 (1) {
  ["val":"DoOperationNoCast":private]=>
  int(31)
}
object(DoOperationNoCast)#3 (1) {
  ["val":"DoOperationNoCast":private]=>
  int(150)
}
