--TEST--
Enum var_dump reference
--FILE--
<?php

enum Foo {
    case Bar;
    case Baz;
}

$arr = [Foo::Bar];
$arr[1] = &$arr[0];
var_dump($arr);

?>
--EXPECT--
array(2) {
  [0]=>
  &enum(Foo::Bar)
  [1]=>
  &enum(Foo::Bar)
}
