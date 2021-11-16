--TEST--
Enum in static var
--FILE--
<?php

enum Foo {
    case Bar;
}

function example() {
    static $bar = Foo::Bar;
    return $bar;
}

var_dump(example());
var_dump(example());

?>
--EXPECT--
enum(Foo::Bar)
enum(Foo::Bar)
