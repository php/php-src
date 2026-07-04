--TEST--
ReflectionClass with wrong-case class name fails with wrong case
--FILE--
<?php
class MyClass {
    public int $value = 42;
}

$rc = new ReflectionClass("myclass");
echo $rc->getName() . "\n";

$rc2 = new ReflectionClass("MYCLASS");
echo $rc2->getName() . "\n";
?>
--EXPECTF--
Fatal error: Uncaught ReflectionException: Class "myclass" does not exist in %s:%d
Stack trace:
#0 %s(6): ReflectionClass->__construct('myclass')
#1 {main}
  thrown in %s on line %d
