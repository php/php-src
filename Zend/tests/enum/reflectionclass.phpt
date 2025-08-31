--TEST--
Enum reflection getConstants()
--FILE--
<?php

enum Foo {
    case Bar;
    case Baz;
}

var_dump((new \ReflectionClass(Foo::class))->getConstants());

?>
--EXPECT--
array(2) {
  ["Bar"]=>
  enum(Foo::Bar)
  ["Baz"]=>
  enum(Foo::Baz)
}
