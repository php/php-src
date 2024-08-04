--TEST--
Abstract hooks in non-abstract class gives an error
--FILE--
<?php

class Test {
    public abstract $prop {
        get;
        set {}
    }
}

?>
--EXPECTF--
Fatal error: Class Test contains 1 abstract method and must therefore be declared abstract or implement the remaining methods (Test::$prop::get) in %s on line %d
