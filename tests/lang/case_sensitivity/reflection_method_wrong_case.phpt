--TEST--
ReflectionMethod with wrong-case class name fails with wrong case
--FILE--
<?php
class MyClass {
    public function myMethod(): void {}
}

$rm = new ReflectionMethod("myclass", "myMethod");
echo $rm->getName() . "\n";

$rm2 = new ReflectionMethod("MYCLASS", "myMethod");
echo $rm2->getName() . "\n";
?>
--EXPECTF--
Fatal error: Uncaught ReflectionException: Class "myclass" does not exist in %s:%d
Stack trace:
#0 %s(6): ReflectionMethod->__construct('myclass', 'myMethod')
#1 {main}
  thrown in %s on line %d
