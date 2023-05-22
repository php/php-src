--TEST--
#[Deprecated] attribute with wrong type
--FILE--
<?php

#[Deprecated(1234)]
function test() {
}


--EXPECTF--
Fatal error: Deprecated::__construct: Argument #1 ($message) must be of type string, int given in %s
