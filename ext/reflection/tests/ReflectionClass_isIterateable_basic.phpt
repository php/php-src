--TEST--
ReflectionClass::isIterateable() basic
--CREDITS--
Felix De Vliegher <felix.devliegher@gmail.com>, Marc Veldman <marc@ibuildings.nl>
--FILE--
<?php

class IteratorClass implements Iterator {
    public function __construct() { }
    public function key(): mixed {}
    public function current(): mixed {}
    function next(): void {}
    function valid(): bool {}
    function rewind(): void {}
}
class DerivedClass extends IteratorClass {}
class RegularClass {}

function dump_iterateable($class) {
    $reflection = new ReflectionClass($class);
    var_dump($reflection->isIterateable());
}

$classes = array("ArrayObject", "IteratorClass", "DerivedClass", "RegularClass");
foreach ($classes as $class) {
    echo "Is $class iterateable? ";
    dump_iterateable($class);
}
?>
--EXPECT--
Is ArrayObject iterateable? bool(true)
Is IteratorClass iterateable? bool(true)
Is DerivedClass iterateable? bool(true)
Is RegularClass iterateable? bool(true)
