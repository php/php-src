--TEST--
Using parent::$prop::get() in a different hook
--FILE--
<?php

class A {
    public $foo {
        set {
            parent::$foo::get();
        }
    }
}

?>
--EXPECTF--
Fatal error: Must not use parent::$foo::get() in a different property hook (set) in %s on line %d
