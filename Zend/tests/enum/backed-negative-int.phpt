--TEST--
Backed enum with negative int
--FILE--
<?php

enum Foo: int {
    case Bar = -1;
    case Baz = -2;
}

var_dump(Foo::Bar->value);
var_dump(Foo::Baz->value);
var_dump(Foo::from(-1));
var_dump(Foo::from(-2));

?>
--EXPECT--
int(-1)
int(-2)
enum(Foo::Bar)
enum(Foo::Baz)
