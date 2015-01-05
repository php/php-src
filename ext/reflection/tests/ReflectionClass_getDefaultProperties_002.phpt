--TEST--
ReflectionClass::getDefaultProperties(), ReflectionClass::getStaticProperties() - wrong param count
--CREDITS--
Robin Fernandes <robinf@php.net>
Steve Seear <stevseea@php.net>
--FILE--
<?php
interface I {}
class C implements I {}
$rc = new ReflectionClass('C');
var_dump($rc->getDefaultProperties(null));
var_dump($rc->getDefaultProperties('X'));
var_dump($rc->getDefaultProperties(true));
var_dump($rc->getDefaultProperties(array(1,2,3)));
var_dump($rc->getStaticProperties(null));
var_dump($rc->getStaticProperties('X'));
var_dump($rc->getStaticProperties(true));
var_dump($rc->getStaticProperties(array(1,2,3)));

?>
--EXPECTF--
Warning: ReflectionClass::getDefaultProperties() expects exactly 0 parameters, 1 given in %s on line %d
NULL

Warning: ReflectionClass::getDefaultProperties() expects exactly 0 parameters, 1 given in %s on line %d
NULL

Warning: ReflectionClass::getDefaultProperties() expects exactly 0 parameters, 1 given in %s on line %d
NULL

Warning: ReflectionClass::getDefaultProperties() expects exactly 0 parameters, 1 given in %s on line %d
NULL

Warning: ReflectionClass::getStaticProperties() expects exactly 0 parameters, 1 given in %s on line %d
NULL

Warning: ReflectionClass::getStaticProperties() expects exactly 0 parameters, 1 given in %s on line %d
NULL

Warning: ReflectionClass::getStaticProperties() expects exactly 0 parameters, 1 given in %s on line %d
NULL

Warning: ReflectionClass::getStaticProperties() expects exactly 0 parameters, 1 given in %s on line %d
NULL
