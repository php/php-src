--TEST--
Test that final properties can be cloned
--FILE--
<?php

class Foo
{
    final public int $property1;
    final public int $property2;
    final public string $property3 = "";
}

$foo = new Foo();
$foo->property2 = 2;
var_dump($foo);
$bar = clone $foo;
var_dump($bar);

?>
--EXPECTF--
object(Foo)#1 (2) {
  ["property1"]=>
  uninitialized(int)
  ["property2"]=>
  int(2)
  ["property3"]=>
  string(0) ""
}
object(Foo)#2 (2) {
  ["property1"]=>
  uninitialized(int)
  ["property2"]=>
  int(2)
  ["property3"]=>
  string(0) ""
}
