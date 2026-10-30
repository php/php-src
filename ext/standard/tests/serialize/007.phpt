--TEST--
serialize() with __sleep() without props
--FILE--
<?php

class Foo {
    private $a = 1;
    function __sleep() {
        $this->a = 2;
    }
}

class FooNull {
    public $a = 1;
    function __sleep() {
        $this->a = 2;
        return null;
    }
}

class Bar {
    public $a = 1;
    function __sleep() {
        $this->a = 2;
        return 1;
    }
}

var_dump(serialize(new Foo()));
var_dump(serialize(new FooNull()));
var_dump(serialize(new Bar()));
?>
--EXPECTF--
string(31) "O:3:"Foo":1:{s:6:"%0Foo%0a";i:2;}"
string(30) "O:7:"FooNull":1:{s:1:"a";i:2;}"

Warning: serialize(): Bar::__sleep() should return an array of property names, or return null/void to delegate to default serialization in %s on line %d
string(26) "O:3:"Bar":1:{s:1:"a";i:2;}"
