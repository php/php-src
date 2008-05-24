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

Warning: Wrong parameter count for ReflectionFunctionAbstract::getDocComment() in %s on line %d
NULL

Warning: Wrong parameter count for ReflectionFunctionAbstract::getDocComment() in %s on line %d
NULL

