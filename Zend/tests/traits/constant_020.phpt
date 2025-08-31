--TEST--
Attributes can be retrieved from trait constants
--FILE--
<?php

class TestAttribute {
    public function __construct(public string $value) {}
}

trait TestTrait {
  #[TestAttribute(value: 123)]
  public const Constant = 42;
}

class TestClass {
  use TestTrait;
}

$reflection = new \ReflectionClass(TestTrait::class);
var_dump($reflection->getReflectionConstant('Constant')->getAttributes('TestAttribute')[0]->getArguments()['value']);

$reflection = new \ReflectionClass(TestClass::class);
var_dump($reflection->getReflectionConstant('Constant')->getAttributes('TestAttribute')[0]->getArguments()['value']);

?>
--EXPECTF--
int(123)
int(123)
