--TEST--
ReflectionMethod::getDeclaringClass()
--SKIPIF--
<?php extension_loaded('reflection') or die('skip'); ?>
--FILE--
<?php

class A {
    function foo() {}
}

class B extends A {
    function bar() {}
}

$methodInfo = new ReflectionMethod('B', 'foo');
var_dump($methodInfo->getDeclaringClass());

$methodInfo = new ReflectionMethod('B', 'bar');
var_dump($methodInfo->getDeclaringClass());

?> 
--EXPECTF--
object(ReflectionClass)#%d (1) {
  ["name"]=>
  string(1) "A"
}
object(ReflectionClass)#%d (1) {
  ["name"]=>
  string(1) "B"
}

