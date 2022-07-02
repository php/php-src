--TEST--
Auto implement BackedEnum interface
--FILE--
<?php

enum Foo {
    case Bar;
}

enum Baz: int {
    case Qux = 0;
}

var_dump(Foo::Bar instanceof BackedEnum);
var_dump(Baz::Qux instanceof BackedEnum);

?>
--EXPECT--
bool(false)
bool(true)
