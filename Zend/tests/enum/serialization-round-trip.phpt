--TEST--
Enum unserialize same instance
--FILE--
<?php

enum Foo {
    case Bar;
}

var_dump(Foo::Bar === unserialize(serialize(Foo::Bar)));

?>
--EXPECT--
bool(true)
