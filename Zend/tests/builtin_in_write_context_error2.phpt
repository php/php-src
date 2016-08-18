--TEST--
Cannot use built-in functions in write context (reference)
--FILE--
<?php

$ref =& strlen("foo");

?>
--EXPECTF--
Fatal error: Cannot use result of built-in function in write context in %s on line %d
