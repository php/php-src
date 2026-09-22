--TEST--
Property hooks in interfaces
--FILE--
<?php

interface I {
    public $prop { get; set; }
}

class C implements I {
}

?>
--EXPECTF--
Fatal error: Class C contains 2 abstract methods and must therefore be declared abstract or implement the remaining methods (I::$prop::get, I::$prop::set) in %s on line %d
