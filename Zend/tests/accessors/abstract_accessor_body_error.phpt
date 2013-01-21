--TEST--
Abstract accessor can't have a body
--FILE--
<?php

class Foo {
    public $foo {
        abstract set { }
    }
}

?>
--EXPECTF--
Fatal error: Abstract function Foo::$foo->set() cannot contain body in %s on line %d
