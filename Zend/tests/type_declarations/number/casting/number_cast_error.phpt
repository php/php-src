--TEST--
Test that a number casting is not supported
--FILE--
<?php

$foo = (number) 12;

?>
--EXPECTF--
Parse error: syntax error, unexpected '12' (T_LNUMBER) in %s on line %d
