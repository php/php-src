--TEST--
Unions between array, objects and strings must fail
--FILE--
<?php

function foo(array & string & stdClass $foo) {}

?>
--EXPECTF--
Fatal error: Cannot require parameters to be stdClass, string and array at the same time in intersection types in %s on line %d
