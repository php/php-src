--TEST--
Return type of parent is not allowed in function
--FILE--
<?php

function test(): parent {}

--EXPECTF--
Fatal error: Cannot declare a return type of parent outside of a class scope in %s on line 3
