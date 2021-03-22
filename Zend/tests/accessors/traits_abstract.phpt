--TEST--
Abstract accessors from trait
--FILE--
<?php

trait T {
    public $prop { abstract get; abstract set; }
}

class C {
    use T;
}

?>
--EXPECTF--
Fatal error: Class C contains 2 abstract methods and must therefore be declared abstract or implement the remaining methods (T::$prop::get, T::$prop::set) in %s on line %d
