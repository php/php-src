--TEST--
Test that immutable classes can be cloned
--FILE--
<?php

immutable class Foo
{
    public int $property1;
    public string $property2 = "";
    protected $property4;
    private $property;
}

$foo = new Foo();
$foo->property1 = 1;
var_dump($foo);
$bar = clone $foo;
var_dump($bar);

?>
--EXPECTF--
object(Foo)#1 (4) {
  ["property1"]=>
  int(1)
  ["property2"]=>
  string(0) ""
  ["property4":protected]=>
  NULL
  ["property":"Foo":private]=>
  NULL
}
object(Foo)#2 (4) {
  ["property1"]=>
  int(1)
  ["property2"]=>
  string(0) ""
  ["property4":protected]=>
  NULL
  ["property":"Foo":private]=>
  NULL
}
