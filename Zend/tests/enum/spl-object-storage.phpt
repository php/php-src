--TEST--
Enum in SplObjectStorage
--FILE--
<?php

enum Foo {
    case Bar;
    case Baz;
    case Qux;
}

$storage = new SplObjectStorage();
$storage[Foo::Bar] = 'Bar';
$storage[Foo::Baz] = 'Baz';

var_dump($storage[Foo::Bar]);
var_dump($storage[Foo::Baz]);

var_dump($storage->contains(Foo::Bar));
var_dump($storage->contains(Foo::Qux));

$serialized = serialize($storage);
var_dump($serialized);

$unserialized = unserialize($serialized);
var_dump($unserialized[Foo::Bar]);
var_dump($unserialized[Foo::Baz]);

?>
--EXPECT--
string(3) "Bar"
string(3) "Baz"
bool(true)
bool(false)
string(112) "O:16:"SplObjectStorage":2:{i:0;a:4:{i:0;E:7:"Foo:Bar";i:1;s:3:"Bar";i:2;E:7:"Foo:Baz";i:3;s:3:"Baz";}i:1;a:0:{}}"
string(3) "Bar"
string(3) "Baz"
