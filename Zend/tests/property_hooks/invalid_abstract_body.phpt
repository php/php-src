--TEST--
Abstract property hook cannot have body
--FILE--
<?php

class Test {
    public abstract $prop {
        get {}
    }
}

?>
--EXPECTF--
Fatal error: Abstract property Test::$prop must specify at least one abstract hook in %s on line %d
