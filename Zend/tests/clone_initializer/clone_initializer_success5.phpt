--TEST--
Test that the "clone with" works with dynamic properties
--FILE--
<?php

class Foo
{
}

$foo = new Foo();
$foo = clone $foo with {bar: 1, baz: ""};
var_dump($foo);

?>
--EXPECT--
object(Foo)#2 (2) {
  ["bar"]=>
  int(1)
  ["baz"]=>
  string(0) ""
}
