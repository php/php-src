--TEST--
Enum unserialize
--FILE--
<?php

enum Foo {
    case Bar;
    case Quux;
}

$bar = unserialize('E:7:"Foo:Bar";');
var_dump($bar);
var_dump($bar === Foo::Bar);

$quux = unserialize('E:8:"Foo:Quux";');
var_dump($quux);
var_dump($quux === Foo::Quux);

?>
--EXPECT--
enum(Foo::Bar)
bool(true)
enum(Foo::Quux)
bool(true)
