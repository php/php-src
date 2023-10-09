--TEST--
Flattened trait constants are retrieved as members of the composing class via Reflection
--FILE--
<?php

trait TestTrait {
  public const Constant = 42;
}

class TestClass {
  use TestTrait;
}

$reflection = new \ReflectionClass(TestTrait::class);
var_dump($reflection->getConstant('Constant'));
var_dump($reflection->getReflectionConstant('Constant')->getDeclaringClass()->getName());

$reflection = new \ReflectionClass(TestClass::class);
var_dump($reflection->getConstant('Constant'));
var_dump($reflection->getReflectionConstant('Constant')->getDeclaringClass()->getName());

?>
--EXPECTF--
int(42)
string(9) "TestTrait"
int(42)
string(9) "TestClass"
