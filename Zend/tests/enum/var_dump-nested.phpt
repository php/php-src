--TEST--
Enum var_dump nested
--FILE--
<?php

enum Foo {
    case Bar;
}

var_dump([[Foo::Bar]]);

?>
--EXPECT--
array(1) {
  [0]=>
  array(1) {
    [0]=>
    enum(Foo::Bar)
  }
}
