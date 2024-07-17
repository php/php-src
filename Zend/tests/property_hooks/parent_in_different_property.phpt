--TEST--
Using parent::$prop::get() in a different property
--FILE--
<?php

class A {
    public $foo {
        get {
            return parent::$bar::get();
        }
    }
}

?>
--EXPECTF--
Fatal error: Must not use parent::$bar::get() in a different property ($foo) in %s on line %d
