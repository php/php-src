--TEST--
array_unique() with ARRAY_UNIQUE_IDENTICAL and enums
--FILE--
<?php

enum Foo {
    case Bar;
    case Baz;
}

$bar = Foo::Bar;
$bar2 = Foo::Bar;

var_dump(array_unique([
    Foo::Bar,
    Foo::Baz,
    Foo::Baz,
    Foo::Bar,
], ARRAY_UNIQUE_IDENTICAL));

var_dump(array_unique([
    Foo::Bar,
    Foo::Bar,
    Foo::Baz,
], ARRAY_UNIQUE_IDENTICAL));

var_dump(array_unique([
    'a' => Foo::Bar,
    'b' => Foo::Baz,
    'c' => Foo::Bar,
], ARRAY_UNIQUE_IDENTICAL));

var_dump(array_unique([
    &$bar,
    Foo::Bar,
    &$bar2,
    Foo::Baz,
], ARRAY_UNIQUE_IDENTICAL));

$value2 = "hello";
$value3 = 0;
$value4 = &$value2;
var_dump(array_unique([
    0,
    &$value4,
    &$value2,
    "hello",
    &$value3,
    $value4
], ARRAY_UNIQUE_IDENTICAL));

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
