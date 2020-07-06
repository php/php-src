--TEST--
Return type of parent is not allowed in function
--FILE--
<?php

function test(): parent {}
--EXPECTF--
Fatal error: "parent" cannot be used in the global scope in %s on line %d
