--TEST--
Argument parsing error #001
--FILE--
<?php
function foo($arg1 string) {}
?>
--EXPECTF--
Parse error: syntax error, unexpected T_STRING, expecting ')' in %sfunction_arguments_001.php on line %d

