--TEST--
call_user_func_array() + array_slice() + named arguments
--FILE--
<?php
call_user_func_array('func', array_slice(array: $a, 1, 2));
?>
--EXPECTF--
Fatal error: Cannot use positional argument after named argument in %s on line %d
