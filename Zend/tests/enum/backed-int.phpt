--TEST--
Int enum value
--FILE--
<?php

enum Foo: int {
    case Bar = 0;
    case Baz = 1;
}

var_dump(Foo::Bar->value);
var_dump(Foo::Baz->value);

?>
--EXPECT--
int(0)
int(1)
