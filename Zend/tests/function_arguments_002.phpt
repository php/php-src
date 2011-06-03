--TEST--
Argument parsing error #002
--FILE--
<?php
function foo($arg1/) {}
?>
--EXPECTF--
Parse error: syntax error, unexpected '/', expecting ')' in %sfunction_arguments_002.php on line %d

