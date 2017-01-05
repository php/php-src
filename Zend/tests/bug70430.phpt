--TEST--
Bug #70430: Stack buffer overflow in zend_language_parser()
--FILE--
<?php

$"*** Testing function() :  ***\n";

?>
--EXPECTF--
Parse error: syntax error, unexpected '"*** Testing function() :  ***' (T_CONSTANT_ENCAPSED_STRING), expecting variable (T_VARIABLE) or '{' or '$' in %s on line %d
