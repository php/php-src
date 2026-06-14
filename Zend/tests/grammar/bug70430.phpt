--TEST--
Bug #70430: Stack buffer overflow in zend_language_parser()
--FILE--
<?php

$"*** Testing function() :  ***\n";

?>
--EXPECTF--
Parse error: syntax error, unexpected double-quoted string "*** Testing function() :  ***\n", expecting variable or "{" or "$" in %s on line %d
