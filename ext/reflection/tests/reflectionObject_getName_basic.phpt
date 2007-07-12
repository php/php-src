--TEST--
ReflectionObject::getName() - basic function test
--FILE--
<?php
$r0 = new ReflectionObject();
var_dump($r0->getName());

$r1 = new ReflectionObject(new stdClass);
var_dump($r1->getName());

class C { }
$myInstance = new C;
$r2 = new ReflectionObject($myInstance);
var_dump($r2->getName());

$r3 = new ReflectionObject($r2);
var_dump($r3->getName());

?>
--EXPECTF--

Warning: ReflectionObject::__construct() expects exactly 1 parameter, 0 given in %s on line 2
string(0) ""
string(8) "stdClass"
string(1) "C"
string(16) "ReflectionObject"

