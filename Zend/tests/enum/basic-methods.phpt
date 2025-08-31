--TEST--
Enum methods
--FILE--
<?php

enum Foo {
    case Bar;
    case Baz;

    public function dump() {
        var_dump($this);
    }
}

Foo::Bar->dump();
Foo::Baz->dump();

?>
--EXPECT--
enum(Foo::Bar)
enum(Foo::Baz)
