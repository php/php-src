--TEST--
Return type of self is not allowed in function
--FILE--
<?php

function test(): self {}
--EXPECTF--
Fatal error: "self" cannot be used in the global scope in %s on line %d
