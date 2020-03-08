--TEST--
Test that final properties can be cloned
--FILE--
<?php

class Foo
{
    final public int $property1;
    final public string $property2 = "";
}

$foo = new Foo();
$foo->property1 = 1;
var_dump($foo);
$bar = clone $foo;
var_dump($bar);

?>
--EXPECTF--
object(Foo)#1 (2) {
  ["property1"]=>
  int(1)
  ["property2"]=>
  string(0) ""
}
object(Foo)#2 (2) {
  ["property1"]=>
  int(1)
  ["property2"]=>
  string(0) ""
}
