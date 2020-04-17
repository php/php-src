--TEST--
Parse error when expression in block expression is missing
--FILE--
<?php

$a = {};

?>
--EXPECTF--
Parse error: syntax error, unexpected '}' in %s on line %d
