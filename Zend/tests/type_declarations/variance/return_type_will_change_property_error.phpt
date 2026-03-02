--TEST--
Test that the ReturnTypeWillChange attribute cannot be used with functions
--FILE--
<?php

class Foo
{
    #[ReturnTypeWillChange]
    public int $bar;
}

?>
--EXPECTF--
Fatal error: Attribute "ReturnTypeWillChange" cannot target property (allowed targets: method) in %s on line %d
