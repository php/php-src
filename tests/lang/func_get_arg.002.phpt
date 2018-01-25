--TEST--
func_get_arg with variable number of args
--FILE--
<?php

function foo($a)
{
	$b = func_get_arg(1);
	var_dump($b);
	$b++;
	var_dump(func_get_arg(1));

}
foo(2, 3);
echo "\n";
?>
--EXPECT--
int(3)
int(3)
