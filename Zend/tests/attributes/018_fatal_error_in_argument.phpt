--TEST--
Don't free uninitialized memory if a fatal error occurs in an attribute argument
--FILE--
<?php

#[Attr(a->b::c)]
function test() {}

?>
--EXPECTF--
Fatal error: Dynamic class names are not allowed in compile-time class constant references in %s on line %d
