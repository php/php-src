--TEST--
Test that the ReturnTypeWillChange attribute cannot target classes
--FILE--
<?php

#[ReturnTypeWillChange]
class Foo
{
}

?>
--EXPECTF--
Fatal error: Attribute "ReturnTypeWillChange" cannot target class (allowed targets: method) in %s on line %d
