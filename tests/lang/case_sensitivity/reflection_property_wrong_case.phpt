--TEST--
ReflectionProperty with wrong-case class name fails with wrong case
--FILE--
<?php
class MyClass {
    public int $value = 42;
}

$rp = new ReflectionProperty("myclass", "value");
echo $rp->getName() . "\n";

$rp2 = new ReflectionProperty("MYCLASS", "value");
echo $rp2->getName() . "\n";
?>
--EXPECTF--
Fatal error: Uncaught ReflectionException: Class "myclass" does not exist in %s:%d
Stack trace:
#0 %s(6): ReflectionProperty->__construct('myclass', 'value')
#1 {main}
  thrown in %s on line %d
