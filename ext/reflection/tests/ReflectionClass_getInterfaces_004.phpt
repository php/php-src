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

Warning: Wrong parameter count for ReflectionClass::getInterfaces() in %s on line 5
NULL

Warning: Wrong parameter count for ReflectionClass::getInterfaces() in %s on line 6
NULL

Warning: Wrong parameter count for ReflectionClass::getInterfaces() in %s on line 7
NULL

Warning: Wrong parameter count for ReflectionClass::getInterfaces() in %s on line 8
NULL
