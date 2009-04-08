--TEST--
ReflectionObject::getConstructor() - invalid params
--FILE--
<?php
class C {}
$rc = new ReflectionObject(new C);
var_dump($rc->getConstructor(null));
var_dump($rc->getConstructor('X'));
var_dump($rc->getConstructor(true));
var_dump($rc->getConstructor(array(1,2,3)));
?>
--EXPECTF--

Warning: Wrong parameter count for ReflectionClass::getConstructor() in %s on line 4
NULL

Warning: Wrong parameter count for ReflectionClass::getConstructor() in %s on line 5
NULL

Warning: Wrong parameter count for ReflectionClass::getConstructor() in %s on line 6
NULL

Warning: Wrong parameter count for ReflectionClass::getConstructor() in %s on line 7
NULL
