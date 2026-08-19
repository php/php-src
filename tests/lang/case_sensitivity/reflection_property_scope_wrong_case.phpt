--TEST--
ReflectionProperty::isReadable() and isWritable() with wrong-case scope name fails with wrong case
--FILE--
<?php
class MyClass {
    public int $pub = 1;
    protected int $prot = 2;
}

$rp = new ReflectionProperty(MyClass::class, "prot");
var_dump($rp->isReadable("MYCLASS"));
var_dump($rp->isWritable("MYCLASS"));
?>
--EXPECTF--
Fatal error: Uncaught Error: Class "MYCLASS" not found in %s:%d
Stack trace:
#0 %s(8): ReflectionProperty->isReadable('MYCLASS')
#1 {main}
  thrown in %s on line %d
