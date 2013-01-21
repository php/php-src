--TEST--
Accessor properties can be declared abstract
--FILE--
<?php

class Test {
    abstract public $foo {
        set;
    }
}

?>
--EXPECTF--
Fatal error: Class Test contains 2 abstract accessors and must be declared abstract or implement the remaining accessors (Test::$foo->set, Test::$foo->unset) in %s on line %d
