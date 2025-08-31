--TEST--
Enum name property
--FILE--
<?php

enum Foo {
    case Bar;
    case Baz;
}

enum IntFoo: int {
    case Bar = 0;
    case Baz = 1;
}

var_dump((new ReflectionClass(Foo::class))->getProperties());
var_dump(Foo::Bar->name);

var_dump((new ReflectionClass(IntFoo::class))->getProperties());
var_dump(IntFoo::Bar->name);

?>
--EXPECT--
array(1) {
  [0]=>
  object(ReflectionProperty)#2 (2) {
    ["name"]=>
    string(4) "name"
    ["class"]=>
    string(3) "Foo"
  }
}
string(3) "Bar"
array(2) {
  [0]=>
  object(ReflectionProperty)#3 (2) {
    ["name"]=>
    string(4) "name"
    ["class"]=>
    string(6) "IntFoo"
  }
  [1]=>
  object(ReflectionProperty)#4 (2) {
    ["name"]=>
    string(5) "value"
    ["class"]=>
    string(6) "IntFoo"
  }
}
string(3) "Bar"
