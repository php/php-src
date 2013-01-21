--TEST--
Abstract and final modifiers are mutually exclusive
--FILE--
<?php

class Test {
    final $foo {
        abstract get;
    }
}

?>
--EXPECTF--
Fatal error: Abstract and final modifiers are mutually exclusive in %s on line %d
