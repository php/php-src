--TEST--
ReflectionClass::getDocComment() - bad params
--CREDITS--
Robin Fernandes <robinf@php.net>
Steve Seear <stevseea@php.net>
--FILE--
<?php
class C {}
$rc = new ReflectionClass('C');
var_dump($rc->getDocComment(null));
var_dump($rc->getDocComment('X'));
var_dump($rc->getDocComment(true));
var_dump($rc->getDocComment(array(1,2,3)));
?>
--EXPECTF--
Warning: ReflectionClass::getDocComment() expects exactly 0 parameters, 1 given in %s on line %d
NULL

Warning: ReflectionClass::getDocComment() expects exactly 0 parameters, 1 given in %s on line %d
NULL

Warning: ReflectionClass::getDocComment() expects exactly 0 parameters, 1 given in %s on line %d
NULL

Warning: ReflectionClass::getDocComment() expects exactly 0 parameters, 1 given in %s on line %d
NULL
