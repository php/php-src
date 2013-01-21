--TEST--
Classes inheriting abstract accessors have to be marked abstract
--FILE--
<?php

class Foo {
    public $foo {
        abstract set;
    }
}

class Bar extends Foo { }

?>
--EXPECTF--
Fatal error: Class Foo contains 2 abstract accessors and must be declared abstract or implement the remaining accessors (Foo::$foo->set, Foo::$foo->unset) in %s on line %d
