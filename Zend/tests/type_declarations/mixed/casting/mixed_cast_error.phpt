--TEST--
Test that a mixed casting is not supported
--FILE--
<?php

$foo = (mixed) 12;

?>
--EXPECTF--
Parse error: syntax error, unexpected integer "12" in %s on line %d
