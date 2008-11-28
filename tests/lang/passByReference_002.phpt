--TEST--
Attempt to pass a constant by reference
--FILE--
<?php

function f(&$arg1)
{
	var_dump($arg1++);
}

f(2);

?>
--EXPECTF--
Fatal error: Only variables can be passed by reference in %s on line 8
