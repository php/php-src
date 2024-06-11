--TEST--
Test that dynamic properties can be initialized during cloning
--FILE--
<?php

class Foo
{
    public function withProperties($property1, $property2)
    {
        return clone $this with [
            "property1" => $property1,
            "property2" => $property2,
        ];
    }
}

$obj1 = new Foo();
$obj2 = $obj1->withProperties(1, false);
$obj3 = $obj2->withProperties(2, true); // The same as above but now using cache slots

var_dump($obj1);
var_dump($obj2);
var_dump($obj3);

?>
--EXPECTF--
Deprecated: Creation of dynamic property Foo::$property1 is deprecated in %s on line %d

Deprecated: Creation of dynamic property Foo::$property2 is deprecated in %s on line %d
object(Foo)#1 (%d) {
}
object(Foo)#2 (%d) {
  ["property1"]=>
  int(1)
  ["property2"]=>
  bool(false)
}
object(Foo)#3 (%d) {
  ["property1"]=>
  int(2)
  ["property2"]=>
  bool(true)
}
