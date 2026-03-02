--TEST--
Enum in constant
--FILE--
<?php

enum Foo {
    case Bar;
}

class Baz {
    const BAR = Foo::Bar;
}

var_dump(Foo::Bar);
var_dump(Baz::BAR);
var_dump(Foo::Bar === Baz::BAR);

?>
--EXPECT--
enum(Foo::Bar)
enum(Foo::Bar)
bool(true)
