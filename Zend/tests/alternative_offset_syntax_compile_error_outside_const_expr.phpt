--TEST--
Alternative offset syntax should emit E_COMPILE_ERROR outside const expression
--FILE--
<?php
$foo = 'BAR';
var_dump($foo{0});
?>
--EXPECTF--
Parse error: syntax error, unexpected token "{", expecting ")" in %s on line %d
