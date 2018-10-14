--TEST--
func_num_args with no args
--FILE--
<?php

function foo()
{
	var_dump(func_num_args());
}
foo();

?>
--EXPECT--
int(0)
