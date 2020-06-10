--TEST--
Enum must not implement Serializable
--FILE--
<?php

enum Foo implements Serializable {
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
Fatal error: Enums may not implement the Serializable interface in %s on line %d
