--TEST--
Enum must not implement Serializable indirectly
--FILE--
<?php

interface MySerializable extends Serializable {}

enum Foo implements MySerializable {
    case Bar;

    public function serialize() {
        return serialize('Hello');
    }

    public function unserialize($data) {
        return unserialize($data);
    }
}

var_dump(unserialize(serialize(Foo::Bar)));

?>
--EXPECTF--
Deprecated: %s implements the Serializable interface, which is deprecated. Implement __serialize() and __unserialize() instead (or in addition, if support for old PHP versions is necessary) in %s on line %d

Fatal error: Enum Foo cannot implement the Serializable interface in %s on line %d
