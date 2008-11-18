--TEST--
Modifiers - wrong param count
--CREDITS--
Robin Fernandes <robinf@php.net>
Steve Seear <stevseea@php.net>
--FILE--
<?php
class C {}
$rc = new ReflectionClass("C");
var_dump($rc->isFinal('X'));
var_dump($rc->isInterface(null));
var_dump($rc->isAbstract(true));
var_dump($rc->getModifiers(array(1,2,3)));

?>
--EXPECTF--

Warning: Wrong parameter count for ReflectionClass::isFinal() in %s on line 4
NULL

Warning: Wrong parameter count for ReflectionClass::isInterface() in %s on line 5
NULL

Warning: Wrong parameter count for ReflectionClass::isAbstract() in %s on line 6
NULL

Warning: Wrong parameter count for ReflectionClass::getModifiers() in %s on line 7
NULL
