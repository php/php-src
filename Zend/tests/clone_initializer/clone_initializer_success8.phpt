--TEST--
Test that "clone with" works with a reference clone operand
--FILE--
<?php

class Foo
{
    public $property1;
    public $property2;
}

$foo = new Foo();
$ref = &$foo;

$bar = clone $ref with ["property1" => 1, "property2" => 2];
var_dump($bar);

?>
--EXPECTF--
object(Foo)#2 (%d) {
  ["property1"]=>
  int(1)
  ["property2"]=>
  int(2)
}