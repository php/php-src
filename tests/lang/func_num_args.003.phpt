--TEST--
func_num_args() outside of a function declaration
--FILE--
<?php

var_dump(func_num_args());

?>
--EXPECTF--
Warning: func_num_args():  Called from the global scope - no function context in %s on line %d
int(-1)
