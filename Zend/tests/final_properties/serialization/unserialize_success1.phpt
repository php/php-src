--TEST--
Test that final properties can be serialized and then unserialized
--FILE--
<?php

class Foo
{
    final public int $property1;
    final public string $property2 = "";
}

$foo = new Foo();

var_dump(unserialize(serialize($foo)));

?>
--EXPECT--
object(Foo)#2 (1) {
  ["property1"]=>
  uninitialized(int)
  ["property2"]=>
  string(0) ""
}
