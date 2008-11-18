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

Warning: Wrong parameter count for ReflectionClass::getDefaultProperties() in %s on line 5
NULL

Warning: Wrong parameter count for ReflectionClass::getDefaultProperties() in %s on line 6
NULL

Warning: Wrong parameter count for ReflectionClass::getDefaultProperties() in %s on line 7
NULL

Warning: Wrong parameter count for ReflectionClass::getDefaultProperties() in %s on line 8
NULL

Warning: Wrong parameter count for ReflectionClass::getStaticProperties() in %s on line 9
NULL

Warning: Wrong parameter count for ReflectionClass::getStaticProperties() in %s on line 10
NULL

Warning: Wrong parameter count for ReflectionClass::getStaticProperties() in %s on line 11
NULL

Warning: Wrong parameter count for ReflectionClass::getStaticProperties() in %s on line 12
NULL