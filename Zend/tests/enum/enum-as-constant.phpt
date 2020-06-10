--TEST--
Enum cases can be referenced by constants
--FILE--
<?php

enum Foo {
    case Bar;
}

const Beep = Foo::Bar;

class Test {
    const Beep = Foo::Bar;
}

var_dump(Beep);
var_dump(Test::Beep);

?>
--EXPECT--
enum(Foo::Bar)
enum(Foo::Bar)
