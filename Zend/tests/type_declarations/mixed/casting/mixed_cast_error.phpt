--TEST--
Test that a mixed casting is not supported
--FILE--
<?php

$foo = (mixed) 12;

?>
--EXPECTF--
Parse error: syntax error, unexpected '12' (T_LNUMBER) in %s on line %d
