--TEST--
Return type of self is not allowed in function

--FILE--
<?php

function test(): self {}

--EXPECTF--
Fatal error: Cannot declare a return type of self outside of a class scope in %s on line 3
