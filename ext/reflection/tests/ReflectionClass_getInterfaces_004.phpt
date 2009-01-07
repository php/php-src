--TEST--
ReflectionClass::getInterfaces() - wrong param count
--CREDITS--
Robin Fernandes <robinf@php.net>
Steve Seear <stevseea@php.net>
--FILE--
<?php
interface I {}
class C implements I {}
$rc = new ReflectionClass('C');
var_dump($rc->getInterfaces(null));
var_dump($rc->getInterfaces('X'));
var_dump($rc->getInterfaces(true));
var_dump($rc->getInterfaces(array(1,2,3)));
?>
--EXPECTF--
Warning: ReflectionClass::getInterfaces() expects exactly 0 parameters, 1 given in %s on line %d
NULL

Warning: ReflectionClass::getInterfaces() expects exactly 0 parameters, 1 given in %s on line %d
NULL

Warning: ReflectionClass::getInterfaces() expects exactly 0 parameters, 1 given in %s on line %d
NULL

Warning: ReflectionClass::getInterfaces() expects exactly 0 parameters, 1 given in %s on line %d
NULL
