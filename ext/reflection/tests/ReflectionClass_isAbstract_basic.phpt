--TEST--
ReflectionClass::isAbstract() method
--CREDITS--
Felix De Vliegher <felix.devliegher@gmail.com>
#testfest roosendaal on 2008-05-10
--FILE--
<?php

class TestClass {}
abstract class TestAbstractClass {}

$testClass = new ReflectionClass('TestClass');
$abstractClass = new ReflectionClass('TestAbstractClass');

var_dump($testClass->isAbstract());
var_dump($abstractClass->isAbstract());

?>
--EXPECT--
bool(false)
bool(true)
