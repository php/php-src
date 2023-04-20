--TEST--
Abstract property hooks from trait
--FILE--
<?php

trait T {
    public abstract $prop { get; set; }
}

class C {
    use T;
}

?>
--EXPECTF--
Fatal error: Class C contains 2 abstract methods and must therefore be declared abstract or implement the remaining methods (C::$prop::get, C::$prop::set) in %s on line %d
