--TEST--
ReflectionClass::isInterface() method
--CREDITS--
Felix De Vliegher <felix.devliegher@gmail.com>
#testfest roosendaal on 2008-05-10
--FILE--
<?php

interface TestInterface {}
class TestClass {}
interface DerivedInterface extends TestInterface {}

$reflectionClass = new ReflectionClass('TestInterface');
$reflectionClass2 = new ReflectionClass('TestClass');
$reflectionClass3 = new ReflectionClass('DerivedInterface');

var_dump($reflectionClass->isInterface());
var_dump($reflectionClass2->isInterface());
var_dump($reflectionClass3->isInterface());

?>
--EXPECT--
bool(true)
bool(false)
bool(true)
