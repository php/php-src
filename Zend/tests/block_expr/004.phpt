--TEST--
Test block expression compilation error when expression is missing
--FILE--
<?php

$a = {};

?>
--EXPECTF--
Fatal error: Block expression must return a value if its result is used in %s on line %d
