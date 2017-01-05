--TEST--
ReflectionObject::isInstance() - invalid params
--FILE--
<?php
class X {}
$instance = new X;
$ro = new ReflectionObject(new X);

var_dump($ro->isInstance());
var_dump($ro->isInstance($instance, $instance));
var_dump($ro->isInstance(1));
var_dump($ro->isInstance(1.5));
var_dump($ro->isInstance(true));
var_dump($ro->isInstance('X'));
var_dump($ro->isInstance(null));

?>
--EXPECTF--
Warning: ReflectionClass::isInstance() expects exactly 1 parameter, 0 given in %s on line 6
NULL

Warning: ReflectionClass::isInstance() expects exactly 1 parameter, 2 given in %s on line 7
NULL

Warning: ReflectionClass::isInstance() expects parameter 1 to be object, integer given in %s on line 8
NULL

Warning: ReflectionClass::isInstance() expects parameter 1 to be object, float given in %s on line 9
NULL

Warning: ReflectionClass::isInstance() expects parameter 1 to be object, boolean given in %s on line 10
NULL

Warning: ReflectionClass::isInstance() expects parameter 1 to be object, string given in %s on line 11
NULL

Warning: ReflectionClass::isInstance() expects parameter 1 to be object, null given in %s on line 12
NULL
