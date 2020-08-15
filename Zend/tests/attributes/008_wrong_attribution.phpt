--TEST--
Attributes: Prevent Attribute on non classes
--FILE--
<?php

#[Attribute]
function foo() {}
?>
--EXPECTF--
Fatal error: Attribute "Attribute" cannot target function (allowed targets: class) in %s
