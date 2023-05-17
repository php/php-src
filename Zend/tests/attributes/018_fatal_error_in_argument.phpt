--TEST--
Don't free uninitialized memory if a fatal error occurs in an attribute argument
--FILE--
<?php

#[Attr(`exit 0`)]
function test() {}

?>
--EXPECTF--
Fatal error: Constant expression contains invalid operations in %s on line %d
