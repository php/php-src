--TEST--
ReflectionClass::isSubclassOf() - invalid number of parameters
--FILE--
<?php
class A {}
$rc = new ReflectionClass('A');

var_dump($rc->isSubclassOf());
var_dump($rc->isSubclassOf('A',5));

?>
--EXPECTF--
Warning: ReflectionClass::isSubclassOf() expects exactly 1 parameter, 0 given in %s on line 5
NULL

Warning: ReflectionClass::isSubclassOf() expects exactly 1 parameter, 2 given in %s on line 6
NULL
