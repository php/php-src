--TEST--
Assoc\unique() and enums
--FILE--
<?php

enum Foo {
    case Bar;
    case Baz;
}

$bar = Foo::Bar;
$bar2 = Foo::Bar;

var_dump(Assoc\unique([
    Foo::Bar,
    Foo::Baz,
    Foo::Baz,
    Foo::Bar,
]));

var_dump(Assoc\unique([
    Foo::Bar,
    Foo::Bar,
    Foo::Baz,
]));

var_dump(Assoc\unique([
    'a' => Foo::Bar,
    'b' => Foo::Baz,
    'c' => Foo::Bar,
]));

var_dump(Assoc\unique([
    &$bar,
    Foo::Bar,
    &$bar2,
    Foo::Baz,
]));

$value2 = "hello";
$value3 = 0;
$value4 = &$value2;
var_dump(Assoc\unique([
    0,
    &$value4,
    &$value2,
    "hello",
    &$value3,
    $value4
]));

?>
--EXPECT--
array(2) {
  [0]=>
  enum(Foo::Bar)
  [1]=>
  enum(Foo::Baz)
}
array(2) {
  [0]=>
  enum(Foo::Bar)
  [2]=>
  enum(Foo::Baz)
}
array(2) {
  ["a"]=>
  enum(Foo::Bar)
  ["b"]=>
  enum(Foo::Baz)
}
array(2) {
  [0]=>
  &enum(Foo::Bar)
  [3]=>
  enum(Foo::Baz)
}
array(2) {
  [0]=>
  int(0)
  [1]=>
  &string(5) "hello"
}
