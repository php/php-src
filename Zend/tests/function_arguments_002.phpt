--TEST--
Argument parsing error #002
--FILE--
<?php
function foo($arg1/) {}
?>
--EXPECTF--
Parse error: %s error, %s)%s in %sfunction_arguments_002.php on line %d

