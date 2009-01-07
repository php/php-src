--TEST--
ReflectionMethod::getDocComment() errors
--FILE--
<?php
class C { function f() {} }
$rc = new ReflectionMethod('C::f');
var_dump($rc->getDocComment(null));
var_dump($rc->getDocComment('X'));
?>
--EXPECTF--
Warning: ReflectionFunctionAbstract::getDocComment() expects exactly 0 parameters, 1 given in %s on line %d
NULL

Warning: ReflectionFunctionAbstract::getDocComment() expects exactly 0 parameters, 1 given in %s on line %d
NULL
