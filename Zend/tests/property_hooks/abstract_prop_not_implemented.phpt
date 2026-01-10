--TEST--
Abstract property not implemented throws an error
--FILE--
<?php

class A {
    abstract public $prop { get; set; }
}

class B extends A {}

?>
--EXPECTF--
Fatal error: Class A contains 2 abstract methods and must therefore be declared abstract or implement the remaining methods (A::$prop::get, A::$prop::set) in %s on line %d
