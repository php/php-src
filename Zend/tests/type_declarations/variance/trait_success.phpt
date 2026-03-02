--TEST--
Trait delayed variance check succeeds
--FILE--
<?php

// Taken from bug #79179.

spl_autoload_register(function() {
    interface InterfaceB extends InterfaceA {}
});

interface InterfaceA {}

trait SomeTrait {
    abstract public function func(): ?InterfaceA;
}

class Foo {
    public function func(): ?InterfaceB {}
}

class Bar extends Foo {
    use SomeTrait;
}

?>
===DONE===
--EXPECT--
===DONE===
