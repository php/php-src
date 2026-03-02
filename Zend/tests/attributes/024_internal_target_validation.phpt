--TEST--
Internal attribute targets are validated.
--FILE--
<?php

#[Attribute]
function a1() { }

?>
--EXPECTF--
Fatal error: Attribute "Attribute" cannot target function (allowed targets: class) in %s
