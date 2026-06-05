--TEST--
ReflectionParameter with wrong-case class name in array callable fails with wrong case
--FILE--
<?php
class MyClass {
    public function myMethod(int $x, string $y): void {}
}

$rp = new ReflectionParameter(["MYCLASS", "myMethod"], 0);
echo $rp->getName() . "\n";

$rp2 = new ReflectionParameter(["myclass", "myMethod"], 1);
echo $rp2->getName() . "\n";
?>
--EXPECTF--
Fatal error: Uncaught ReflectionException: Class "MYCLASS" does not exist in %s:%d
Stack trace:
#0 %s(6): ReflectionParameter->__construct(Array, 0)
#1 {main}
  thrown in %s on line %d
