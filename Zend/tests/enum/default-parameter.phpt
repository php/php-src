--TEST--
Enum in default parameter
--FILE--
<?php

enum Foo {
    case Bar;
}

function baz(Foo $foo = Foo::Bar) {
    var_dump($foo);
}

baz();

?>
--EXPECT--
enum(Foo::Bar)
