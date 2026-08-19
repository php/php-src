--TEST--
ReflectionClass::getAttributes() with wrong-case class name and IS_INSTANCEOF fails with wrong case
--FILE--
<?php
#[Attribute(Attribute::TARGET_CLASS)]
class MyAttr {}

#[MyAttr]
class Foo {}

$rc = new ReflectionClass(Foo::class);

$attrs = $rc->getAttributes("MYATTR", ReflectionAttribute::IS_INSTANCEOF);
echo "count: " . count($attrs) . "\n";

$attrs2 = $rc->getAttributes("myattr", ReflectionAttribute::IS_INSTANCEOF);
echo "count: " . count($attrs2) . "\n";
?>
--EXPECTF--
Fatal error: Uncaught Error: Class "MYATTR" not found in %s:%d
Stack trace:
#0 %s(10): ReflectionClass->getAttributes('MYATTR', 2)
#1 {main}
  thrown in %s on line %d
