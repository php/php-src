--TEST--
ReflectionClass::isInstance() - invalid params
--FILE--
<?php
class X {}

$rc = new ReflectionClass("X");
$instance = new X;

var_dump($rc->isInstance());
var_dump($rc->isInstance($instance, $instance));
var_dump($rc->isInstance(1));
var_dump($rc->isInstance(1.5));
var_dump($rc->isInstance(true));
var_dump($rc->isInstance('X'));
var_dump($rc->isInstance(null));

?>
--EXPECTF--
Warning: ReflectionClass::isInstance() expects exactly 1 parameter, 0 given in %s on line 7
NULL

Warning: ReflectionClass::isInstance() expects exactly 1 parameter, 2 given in %s on line 8
NULL

Warning: ReflectionClass::isInstance() expects parameter 1 to be object, %s given in %s on line 9
NULL

Warning: ReflectionClass::isInstance() expects parameter 1 to be object, float given in %s on line 10
NULL

Warning: ReflectionClass::isInstance() expects parameter 1 to be object, bool given in %s on line 11
NULL

Warning: ReflectionClass::isInstance() expects parameter 1 to be object, string given in %s on line 12
NULL

Warning: ReflectionClass::isInstance() expects parameter 1 to be object, null given in %s on line 13
NULL
