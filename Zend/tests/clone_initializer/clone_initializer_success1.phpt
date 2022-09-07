--TEST--
Test that declared properties can be initialized during cloning
--FILE--
<?php

class Foo
{
    public int $property1;
    public string $property2;

    public function withProperties($property1, $property2)
    {
        return clone $this with {
            property1: $property1,
            property2: $property2,
        };
    }
}

$obj1 = new Foo();
$obj2 = $obj1->withProperties(1, "foo");
$obj3 = $obj2->withProperties(2, "bar"); // The same as above but now using cache slots

var_dump($obj1);
var_dump($obj2);
var_dump($obj3);

?>
--EXPECTF--
object(Foo)#1 (%d) {
  ["property1"]=>
  uninitialized(int)
  ["property2"]=>
  uninitialized(string)
}
object(Foo)#2 (%d) {
  ["property1"]=>
  int(1)
  ["property2"]=>
  string(3) "foo"
}
object(Foo)#3 (%d) {
  ["property1"]=>
  int(2)
  ["property2"]=>
  string(3) "bar"
}
