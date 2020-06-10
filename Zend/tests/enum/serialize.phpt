--TEST--
Enum serialize
--FILE--
<?php

enum Foo {
    case Bar;
}

echo serialize(Foo::Bar);

?>
--EXPECT--
E:7:"Foo:Bar";
