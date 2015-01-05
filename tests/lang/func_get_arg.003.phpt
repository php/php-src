--TEST--
func_get_arg outside of a function declaration
--FILE--
<?php

var_dump (func_get_arg(0));

?>
--EXPECTF--
Warning: func_get_arg():  Called from the global scope - no function context in %s on line %d
bool(false)
