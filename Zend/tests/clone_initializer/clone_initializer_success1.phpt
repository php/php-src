--TEST--
Test that declared properties can be initialized during cloning
--FILE--
<?php

class Foo
{
    public int $property1;
    public string $property2;

    public function withProperties()
    {
        return clone $this with {
            property1: 1,
            property2: "foo",
        };
    }
}

$foo = new Foo();
var_dump($foo);
$bar = $foo->withProperties();
var_dump($bar);

?>
--EXPECT--
object(Foo)#1 (0) {
  ["property1"]=>
  uninitialized(int)
  ["property2"]=>
  uninitialized(string)
}
object(Foo)#2 (2) {
  ["property1"]=>
  int(1)
  ["property2"]=>
  string(3) "foo"
}
