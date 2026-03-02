--TEST--
Int enum const expr
--FILE--
<?php

enum Foo: int {
    case Bar = 1 << 0;
    case Baz = 1 << 1;
    case Qux = 1 << 2;
}

var_dump(Foo::Bar->value);
var_dump(Foo::Baz->value);
var_dump(Foo::Qux->value);

?>
--EXPECT--
int(1)
int(2)
int(4)
