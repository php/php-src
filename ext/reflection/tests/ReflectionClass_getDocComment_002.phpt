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

Warning: Wrong parameter count for ReflectionClass::getDocComment() in %s on line 4
NULL

Warning: Wrong parameter count for ReflectionClass::getDocComment() in %s on line 5
NULL

Warning: Wrong parameter count for ReflectionClass::getDocComment() in %s on line 6
NULL

Warning: Wrong parameter count for ReflectionClass::getDocComment() in %s on line 7
NULL
