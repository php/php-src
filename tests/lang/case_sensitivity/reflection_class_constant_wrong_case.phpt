--TEST--
ReflectionClassConstant with wrong-case class name fails with wrong case
--FILE--
<?php
class MyClass {
    const MY_CONST = "hello";
}

$rcc = new ReflectionClassConstant("myclass", "MY_CONST");
echo $rcc->getValue() . "\n";

$rcc2 = new ReflectionClassConstant("MYCLASS", "MY_CONST");
echo $rcc2->getValue() . "\n";
?>
--EXPECTF--
Fatal error: Uncaught ReflectionException: Class "myclass" does not exist in %s:%d
Stack trace:
#0 %s(6): ReflectionClassConstant->__construct('myclass', 'MY_CONST')
#1 {main}
  thrown in %s on line %d
