--TEST--
Test that the ReturnTypeWillChange attribute cannot target functions
--FILE--
<?php

#[ReturnTypeWillChange]
function foo() {}

?>
--EXPECTF--
Fatal error: Attribute "ReturnTypeWillChange" cannot target function (allowed targets: method) in %s on line %d
