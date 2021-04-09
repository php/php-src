--TEST--
Enum var_export
--FILE--
<?php

enum Foo {
    case Bar;
}

var_export(Foo::Bar);

?>
--EXPECT--
Foo::Bar
