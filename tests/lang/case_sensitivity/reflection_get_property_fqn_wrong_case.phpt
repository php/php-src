--TEST--
ReflectionClass::getProperty() with fully-qualified name and wrong-case class fails with wrong case
--FILE--
<?php
class Base {
    protected int $value = 0;
}

class Child extends Base {}

$rc = new ReflectionClass(Child::class);

// Correct case works
$rp = $rc->getProperty("Base::value");
echo $rp->getName() . "\n";

// Wrong case fails
$rp2 = $rc->getProperty("BASE::value");
echo $rp2->getName() . "\n";
?>
--EXPECTF--
value

Fatal error: Uncaught ReflectionException: Class "BASE" does not exist in %s:%d
Stack trace:
#0 %s(%d): ReflectionClass->getProperty('BASE::value')
#1 {main}
  thrown in %s on line %d
