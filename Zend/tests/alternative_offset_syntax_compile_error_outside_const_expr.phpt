--TEST--
Alternative offset syntax should emit E_COMPILE_ERROR outside const expression
--FILE--
<?php
$foo = 'BAR';
var_dump($foo{0});
?>
--EXPECTF--
Fatal error: Array and string offset access syntax with curly braces is no longer supported in %s on line 3
